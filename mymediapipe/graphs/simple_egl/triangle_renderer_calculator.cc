#include <memory>
#include <string>
#include <vector>
#include "absl/types/optional.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mymediapipe/graphs/simple_egl/triangle_renderer_calculator.pb.h"
#include "mymediapipe/graphs/simple_egl/libs/triangle_renderer.h"

namespace mediapipe {
namespace {

static constexpr char kImageGpuTag[] = "IMAGE_GPU";

class TriangleRendererCalculator : public CalculatorBase {
public:
    static absl::Status GetContract(CalculatorContract* cc) {
        MP_RETURN_IF_ERROR(mediapipe::GlCalculatorHelper::UpdateContract(cc))
            << "Failed to update contract for the GPU helper!";
        
        cc->Inputs().Tag(kImageGpuTag).Set<GpuBuffer>();
        cc->Outputs().Tag(kImageGpuTag).Set<GpuBuffer>();
        return mediapipe::GlCalculatorHelper::UpdateContract(cc);
    }

    absl::Status Open(CalculatorContext* cc) override {
        cc->SetOffset(mediapipe::TimestampDiff(0));
        MP_RETURN_IF_ERROR(gpu_helper_.Open(cc))
            << "Failed to open the GPU helper!";

        return gpu_helper_.RunInGlContext([&]() -> absl::Status {
            const auto& options = cc->Options<TriangleRendererCalculatorOptions>();

            LOG(INFO) << "TriangleRendererCalculator.Open -- CreateTriangleRenderer()";

            ASSIGN_OR_RETURN(effect_renderer_,
                             CreateTriangleRenderer(),
                             _ << "Failed to create the effect renderer!");
            return absl::OkStatus();
        });
    }

    absl::Status Process(CalculatorContext* cc) override {
        // The `IMAGE_GPU` stream is required to have a non-empty packet. In case
        // this requirement is not met, there's nothing to be processed at the
        // current timestamp.
        if (cc->Inputs().Tag(kImageGpuTag).IsEmpty()) {
            return absl::OkStatus();
        }

        // LOG(INFO) << "TriangleRendererCalculator.Process()";

        return gpu_helper_.RunInGlContext([this, cc]() -> absl::Status {
            const auto& input_gpu_buffer = cc->Inputs().Tag(kImageGpuTag).Get<GpuBuffer>();

            GlTexture input_gl_texture = gpu_helper_.CreateSourceTexture(input_gpu_buffer);
            // LOG(INFO) << "TriangleRendererCalculator -- input_gl_texture";

            GlTexture output_gl_texture = gpu_helper_.CreateDestinationTexture(
                input_gl_texture.width(), input_gl_texture.height());
            // LOG(INFO) << "TriangleRendererCalculator -- output_gl_texture";

            MP_RETURN_IF_ERROR(effect_renderer_->RenderEffect(
                                                    input_gl_texture.width(), input_gl_texture.height(), 
                                                    input_gl_texture.target(), input_gl_texture.name(), 
                                                    output_gl_texture.target(), output_gl_texture.name()
                                                )) << "Failed to render the effect!";

            // LOG(INFO) << "TriangleRendererCalculator.Process()";

            // auto output_gpu_buffer = std::make_unique<GpuBuffer>(input_gpu_buffer);
            std::unique_ptr<GpuBuffer> output_gpu_buffer = output_gl_texture.GetFrame<GpuBuffer>();

            cc->Outputs()
                .Tag(kImageGpuTag)
                .Add(output_gpu_buffer.release(), cc->InputTimestamp());
            // output_gl_texture.Release();
            // input_gl_texture.Release();

            return absl::OkStatus();
        });
    }

    ~TriangleRendererCalculator() {
        gpu_helper_.RunInGlContext([this]() { effect_renderer_.reset(); });
    }
private:
    mediapipe::GlCalculatorHelper gpu_helper_;
    std::unique_ptr<TriangleRenderer> effect_renderer_;
};

}  // namespace

REGISTER_CALCULATOR(TriangleRendererCalculator);

}  // namespace mediapipe