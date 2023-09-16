#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_calculator.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum { ATTRIB_VERTEX, ATTRIB_TEXTURE_POSITION, NUM_ATTRIBUTES };

namespace mediapipe {

// Lens Flare Example 
// https://www.shadertoy.com/view/4sX3Rs
// This is free and unencumbered software released into the public domain. https://unlicense.org/
// Trying to get some interesting looking lens flares, seems like it worked. 
// See https://www.shadertoy.com/view/lsBGDK for a more avanced, generalized solution
// If you find this useful send me an email at peterekepeter at gmail dot com, 
// I've seen this shader pop up before in other works, but I'm curious where it ends up.
// If you want to use it, feel free to do so, there is no need to credit but it is appreciated.
class LensFlareShaderCalculator : public CalculatorBase {
public:
    LensFlareShaderCalculator() : initialized_(false) {}
    LensFlareShaderCalculator(const LensFlareShaderCalculator&) = delete;
    LensFlareShaderCalculator& operator=(const LensFlareShaderCalculator&) = delete;
    ~LensFlareShaderCalculator() override = default;

    static absl::Status GetContract(CalculatorContract* cc);
    absl::Status Open(CalculatorContext* cc) override;
    absl::Status Process(CalculatorContext* cc) override;
    absl::Status Close(CalculatorContext* cc) override;
    
    absl::Status GlSetup();
    absl::Status GlBind();
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp);
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
    GLint frame_;
    GLint projection_;
    GLint model_;
    GLint texture_;
};

REGISTER_CALCULATOR(LensFlareShaderCalculator);

// static
absl::Status LensFlareShaderCalculator::GetContract(CalculatorContract* cc) {
  TagOrIndex(&cc->Inputs(), "VIDEO", 0).Set<GpuBuffer>();
  TagOrIndex(&cc->Outputs(), "VIDEO", 0).Set<GpuBuffer>();
  // Currently we pass GL context information and other stuff as external
  // inputs, which are handled by the helper.
  return GlCalculatorHelper::UpdateContract(cc);
}

absl::Status LensFlareShaderCalculator::Open(CalculatorContext* cc) {
  // Inform the framework that we always output at the same timestamp
  // as we receive a packet at.
  cc->SetOffset(mediapipe::TimestampDiff(0));

  // Let the helper access the GL context information.
  return helper_.Open(cc);
}

absl::Status LensFlareShaderCalculator::Process(CalculatorContext* cc) {
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

absl::Status LensFlareShaderCalculator::Close(CalculatorContext* cc) {
  return RunInGlContext([this]() -> absl::Status { return GlTeardown(); });
}

absl::Status LensFlareShaderCalculator::GlSetup() {
    // Load vertex and fragment shaders
    const GLint attr_location[NUM_ATTRIBUTES] = {
        ATTRIB_VERTEX,
        ATTRIB_TEXTURE_POSITION,
    };

    const GLchar* attr_name[NUM_ATTRIBUTES] = {
        "position",
        "tex_coord",
    };

    const GLchar* vert_src = R"(
        uniform mat4 projection_mat;
        uniform mat4 model_mat;

        attribute vec4 position;
        attribute vec4 tex_coord;

        varying vec2 v_tex_coord;

        void main() {
            v_tex_coord = tex_coord.xy;
            gl_Position = projection_mat * model_mat * position;
        }
    )";
    const GLchar* frag_src = R"(
        precision mediump float;

        varying vec2 v_tex_coord;
        uniform sampler2D texture;

        void main() {
            gl_FragColor = texture2D(texture, v_tex_coord);
        }
    )";

    // shader program
    GlhCreateProgram(vert_src, frag_src, NUM_ATTRIBUTES,
                    (const GLchar**)&attr_name[0], attr_location, &program_);
    RET_CHECK(program_) << "Problem initializing the program.";

    model_ = glGetUniformLocation(program_, "model_mat");
    projection_ = glGetUniformLocation(program_, "projection_mat");
    texture_ = glGetUniformLocation(program_, "texture");

    RET_CHECK_NE(projection_, -1)
        << "Failed to find `projection_mat` uniform!";
    RET_CHECK_NE(model_, -1)
        << "Failed to find `model_mat` uniform!";
    RET_CHECK_NE(texture_, -1) << "Failed to find `texture` uniform!";
    
    return absl::OkStatus();
}

absl::Status LensFlareShaderCalculator::GlBind() {
    return absl::OkStatus();
}

absl::Status LensFlareShaderCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                                 double timestamp) {

    return absl::OkStatus();
}

absl::Status LensFlareShaderCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}

}  // namespace mediapipe