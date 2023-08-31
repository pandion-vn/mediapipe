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
        cc->Outputs().Tag(kImageGpuTag).Set<GpuBuffer>()
        ;
        return mediapipe::GlCalculatorHelper::UpdateContract(cc);
    }

    absl::Status Open(CalculatorContext* cc) override {
        cc->SetOffset(mediapipe::TimestampDiff(0));
        MP_RETURN_IF_ERROR(gpu_helper_.Open(cc))
            << "Failed to open the GPU helper!";

        return gpu_helper_.RunInGlContext([&]() -> absl::Status {
            const auto& options = cc->Options<TriangleRendererCalculatorOptions>();
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

        return gpu_helper_.RunInGlContext([this, cc]() -> absl::Status {
            const auto& input_gpu_buffer = cc->Inputs().Tag(kImageGpuTag).Get<GpuBuffer>();

            std::unique_ptr<GpuBuffer> output_gpu_buffer = std::make_unique<GpuBuffer>(input_gpu_buffer);

            cc->Outputs()
                .Tag(kImageGpuTag)
                .AddPacket(mediapipe::Adopt<GpuBuffer>(output_gpu_buffer.release())
                .At(cc->InputTimestamp()));
            return absl::OkStatus();
        });
    }

    ~TriangleRendererCalculator() {
        // gpu_helper_.RunInGlContext([this]() { effect_renderer_.reset(); });
    }
private:
    mediapipe::GlCalculatorHelper gpu_helper_;
    std::unique_ptr<TriangleRenderer> effect_renderer_;
};

}  // namespace

REGISTER_CALCULATOR(TriangleRendererCalculator);

}  // namespace mediapipe