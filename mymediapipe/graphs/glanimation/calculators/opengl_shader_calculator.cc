#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_calculator.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"

namespace mediapipe {

enum { ATTRIB_VERTEX, ATTRIB_TEXTURE_POSITION, NUM_ATTRIBUTES };

class OpenGlShaderCalculator : public CalculatorBase {
public:
    OpenGlShaderCalculator() : initialized_(false) {}
    OpenGlShaderCalculator(const OpenGlShaderCalculator&) = delete;
    OpenGlShaderCalculator& operator=(const OpenGlShaderCalculator&) = delete;
    ~OpenGlShaderCalculator() override = default;

    static absl::Status GetContract(CalculatorContract* cc);
    absl::Status Open(CalculatorContext* cc) override;
    absl::Status Process(CalculatorContext* cc) override;
    absl::Status Close(CalculatorContext* cc) override;

    absl::Status GlSetup();
    absl::Status GlBind();
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp);
    absl::Status GlRenderDrawArrays(GLint& positionLocation);
    absl::Status GlRenderDrawElements(GLint& positionLocation);
    absl::Status GlTeardown();

    GpuBufferFormat GetOutputFormat() { return GpuBufferFormat::kBGRA32; }

protected:
    // Forward invocations of RunInGlContext to the helper.
    // The decltype part just says that this method returns whatever type the
    // helper method invocation returns. In C++14 we could remove it and use
    // return type deduction, i.e.:
    //   template <typename F> auto RunInGlContext(F&& f) { ... }
    template <typename F>
    auto RunInGlContext(F&& f)
        -> decltype(std::declval<GlCalculatorHelper>().RunInGlContext(f)) {
    return helper_.RunInGlContext(std::forward<F>(f));
    }

    GlCalculatorHelper helper_;
    bool initialized_;

private:
    GLuint program_ = 0;
    GLint texture_;

};

REGISTER_CALCULATOR(OpenGlShaderCalculator);

// static
absl::Status OpenGlShaderCalculator::GetContract(CalculatorContract* cc) {
  TagOrIndex(&cc->Inputs(), "VIDEO", 0).Set<GpuBuffer>();
  TagOrIndex(&cc->Outputs(), "VIDEO", 0).Set<GpuBuffer>();
  // Currently we pass GL context information and other stuff as external
  // inputs, which are handled by the helper.
  return GlCalculatorHelper::UpdateContract(cc);
}

absl::Status OpenGlShaderCalculator::Open(CalculatorContext* cc) {
  // Inform the framework that we always output at the same timestamp
  // as we receive a packet at.
  cc->SetOffset(mediapipe::TimestampDiff(0));

  // Let the helper access the GL context information.
  return helper_.Open(cc);
}

absl::Status OpenGlShaderCalculator::Process(CalculatorContext* cc) {
  return RunInGlContext([this, cc]() -> absl::Status {
    const auto& input = TagOrIndex(cc->Inputs(), "VIDEO", 0).Get<GpuBuffer>();
    if (!initialized_) {
        MP_RETURN_IF_ERROR(GlSetup());
        initialized_ = true;
    }

    auto src = helper_.CreateSourceTexture(input);
    auto dst = helper_.CreateDestinationTexture(src.width(), src.height(),
                                                GetOutputFormat());

    MP_RETURN_IF_ERROR(GlBind());
    // Run core program.
    MP_RETURN_IF_ERROR(GlRender(src, dst, cc->InputTimestamp().Seconds()));

    auto output = dst.GetFrame<GpuBuffer>();

    src.Release();
    dst.Release();

    TagOrIndex(&cc->Outputs(), "VIDEO", 0)
        .Add(output.release(), cc->InputTimestamp());

    return absl::OkStatus();
  });
}

absl::Status OpenGlShaderCalculator::Close(CalculatorContext* cc) {
  return RunInGlContext([this]() -> absl::Status { return GlTeardown(); });
}

absl::Status OpenGlShaderCalculator::GlSetup() {
    return absl::OkStatus();
}

absl::Status OpenGlShaderCalculator::GlBind() {
    return absl::OkStatus();
}

absl::Status OpenGlShaderCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                          double timestamp) {
    return absl::OkStatus();    
}

absl::Status OpenGlShaderCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}

} // mediapipe