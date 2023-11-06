#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/status_macros.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
// #include "lib/framebuffer_target.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
// #include "gl_base_calculator.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "mediapipe/framework/formats/matrix_data.pb.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/modules/face_geometry/protos/environment.pb.h"
#include "mediapipe/modules/face_geometry/protos/face_geometry.pb.h"
#include "Eigen/Core"

// #include "lib/stb_image.h"
// #include "lib/shader.h"
// #include "lib/camera.h"
// #include "lib/model.h"
// #include "lib/video_scene.h"
// #include "lib/model_scene_a.h"
// #include "lib/model_line.h"
// #include "lib/animation.h"
// #include "lib/animator.h"
// #include "lib/util.h"
// #include "lib/my_pose.h"
// #include "lib/kalidokit.h"

namespace mediapipe {

static constexpr char kImageGpuTag[] = "IMAGE_GPU";

class GlIkOpenglCalculator : public CalculatorBase {
public:
    GlIkOpenglCalculator() : initialized_(false) {}
    GlIkOpenglCalculator(const GlIkOpenglCalculator&) = delete;
    GlIkOpenglCalculator& operator=(const GlIkOpenglCalculator&) = delete;
    ~GlIkOpenglCalculator();

    static absl::Status GetContract(CalculatorContract* cc);
    absl::Status Open(CalculatorContext* cc) override;
    absl::Status Process(CalculatorContext* cc) override;
    absl::Status Close(CalculatorContext* cc) override;
protected:
    // Forward invocations of RunInGlContext to the helper.
    // The decltype part just says that this method returns whatever type the
    // helper method invocation returns. In C++14 we could remove it and use
    // return type deduction, i.e.:
    //   template <typename F> auto RunInGlContext(F&& f) { ... }
    template <typename F>
    auto RunInGlContext(F&& f)
        -> decltype(std::declval<GlCalculatorHelper>().RunInGlContext(f)) {
        return gpu_helper_.RunInGlContext(std::forward<F>(f));
    }

private:
    bool initialized_;
    GlCalculatorHelper gpu_helper_;
};

REGISTER_CALCULATOR(GlIkOpenglCalculator);

// A calculator that renders a visual effect for multiple faces.
//
// Inputs:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer containing input image.
// Output:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer with a visual effect being rendered for multiple faces.
// static
absl::Status GlIkOpenglCalculator::GetContract(CalculatorContract* cc) {
    MP_RETURN_IF_ERROR(GlCalculatorHelper::UpdateContract(cc))
        << "Failed to update contract for the GPU helper!";

    cc->Inputs().Tag(kImageGpuTag).Set<GpuBuffer>();
    
    // cc->InputSidePackets()
    //         .Tag(kEnvironmentTag)
    //         .Set<face_geometry::Environment>();

    // if (cc->Inputs().HasTag(kWorldLandmarksTag)) {
    //     cc->Inputs().Tag(kWorldLandmarksTag).Set<LandmarkList>();
    // }

    // if (cc->Inputs().HasTag(kLandmarksTag)) {
    //     cc->Inputs().Tag(kLandmarksTag).Set<NormalizedLandmarkList>();
    // }
    
    // if (cc->Inputs().HasTag(kMultiFaceGeometryTag)) {
    //     cc->Inputs()
    //             .Tag(kMultiFaceGeometryTag)
    //             .Set<std::vector<face_geometry::FaceGeometry>>();
    // }

    cc->Outputs().Tag(kImageGpuTag).Set<GpuBuffer>();
    // Currently we pass GL context information and other stuff as external
    // inputs, which are handled by the helper.
    return GlCalculatorHelper::UpdateContract(cc);
}

absl::Status GlIkOpenglCalculator::Open(CalculatorContext* cc) {
    // Inform the framework that we always output at the same timestamp
    // as we receive a packet at.
    cc->SetOffset(mediapipe::TimestampDiff(0));

    // Let the helper access the GL context information.
    MP_RETURN_IF_ERROR(gpu_helper_.Open(cc)) 
        << "Failed to open the GPU helper!";
    return RunInGlContext([this, cc]() -> absl::Status {
        // environment_ = cc->InputSidePackets()
        //                     .Tag(kEnvironmentTag)
        //                     .Get<face_geometry::Environment>();

        // ASSIGN_OR_RETURN(framebuffer_target_, FrameBufferTarget::Create(),
        //                  _ << "Failed to create a framebuffer target!");
        return absl::OkStatus();
    });
}

absl::Status GlIkOpenglCalculator::Process(CalculatorContext* cc) {
    // The `IMAGE_GPU` stream is required to have a non-empty packet. In case
    // this requirement is not met, there's nothing to be processed at the
    // current timestamp.
    if (cc->Inputs().Tag(kImageGpuTag).IsEmpty()) {
        return absl::OkStatus();
    }

    return RunInGlContext([this, cc]() -> absl::Status {
        const auto& input_gpu_buffer = cc->Inputs().Tag(kImageGpuTag).Get<GpuBuffer>();
        if (!initialized_) {
            // MP_RETURN_IF_ERROR(GlSetup());
            initialized_ = true;
            // animation_start_time_ = cc->InputTimestamp().Seconds();
        }

        GlTexture input_gl_texture = gpu_helper_.CreateSourceTexture(input_gpu_buffer);
        int dst_width = 1440; // input_gl_texture.width();
        int dst_height = 900; // input_gl_texture.height();
        GlTexture output_gl_texture = gpu_helper_.CreateDestinationTexture(dst_width, dst_height);

        // Set the destination texture as the color buffer. Then, clear both the
        // color and the depth buffers for the render target.
        // framebuffer_target_->SetColorbuffer(dst_width, dst_height, output_gl_texture.target(), output_gl_texture.name());
        // framebuffer_target_->Clear();

        // MP_RETURN_IF_ERROR(GlBind());
        // // Run core program.
        // MP_RETURN_IF_ERROR(GlRender(cc,
        //                             input_gl_texture, 
        //                             output_gl_texture, 
        //                             cc->InputTimestamp().Seconds()));

        // MP_RETURN_IF_ERROR(GlCleanup());
        
        std::unique_ptr<GpuBuffer> output_gpu_buffer = output_gl_texture.GetFrame<GpuBuffer>();

        cc->Outputs()
            .Tag(kImageGpuTag)
            .AddPacket(mediapipe::Adopt<GpuBuffer>(output_gpu_buffer.release())
                            .At(cc->InputTimestamp()));

        output_gl_texture.Release();
        input_gl_texture.Release();

        return absl::OkStatus();
    });
}

absl::Status GlIkOpenglCalculator::Close(CalculatorContext* cc) {
    return RunInGlContext([this]() -> absl::Status { 
        // return GlTeardown(); 
        return absl::OkStatus();
    });
}

GlIkOpenglCalculator::~GlIkOpenglCalculator() {
  RunInGlContext([this] {
    // framebuffer_target_.reset();
  });
}

}