#include <memory>
#include <string>
#include <vector>
#include "absl/types/optional.h"
#include "mediapipe/framework/calculator_framework.h"

namespace mediapipe {
namespace {

static constexpr char kImageGpuTag[] = "IMAGE_GPU";

class TriangleRendererCalculator : public CalculatorBase {
public:
    static absl::Status GetContract(CalculatorContract* cc) {
        return absl::OkStatus();
    }

    absl::Status Open(CalculatorContext* cc) override {
        return absl::OkStatus();
    }

    absl::Status Process(CalculatorContext* cc) override {
        return absl::OkStatus();
    }

    ~TriangleRendererCalculatorCalculator() {
    }
private:
    mediapipe::GlCalculatorHelper gpu_helper_;
};

}  // namespace

REGISTER_CALCULATOR(TriangleRendererCalculator);

}  // namespace mediapipe