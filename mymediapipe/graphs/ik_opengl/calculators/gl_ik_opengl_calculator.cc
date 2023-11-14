#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/status_macros.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
// #include "gl_base_calculator.h"
#include "mymediapipe/calculators/gpu/framebuffer_target.h"
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
#include "mymediapipe/calculators/ik/camera.h"
#include "mymediapipe/calculators/ik/target.h"
#include "mymediapipe/calculators/ik/chain.h"
#include "mymediapipe/calculators/gpu/video_scene.h"

namespace mediapipe {

static constexpr char kImageGpuTag[] = "IMAGE_GPU";

class GlIkOpenglCalculator : public CalculatorBase {
public:
    GlIkOpenglCalculator() : initialized_(false), count_(0) {}
    GlIkOpenglCalculator(const GlIkOpenglCalculator&) = delete;
    GlIkOpenglCalculator& operator=(const GlIkOpenglCalculator&) = delete;
    ~GlIkOpenglCalculator();

    static absl::Status GetContract(CalculatorContract* cc);
    absl::Status Open(CalculatorContext* cc) override;
    absl::Status Process(CalculatorContext* cc) override;
    absl::Status Close(CalculatorContext* cc) override;

    absl::Status GlSetup();
    absl::Status GlBind();
    absl::Status GlRender(CalculatorContext* cc, const GlTexture& src, const GlTexture& dst, double timestamp);
    absl::Status GlCleanup();
    absl::Status GlTeardown();
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
    std::unique_ptr<FrameBufferTarget> framebuffer_target_;

private:
    bool initialized_;
    int count_;
    Camera* camera;
    GlCalculatorHelper gpu_helper_;
    std::vector<glm::vec3> joints1;
    std::vector<glm::vec3> joints2;
    Chain* chain1;
    Chain* chain2;
    Target* target;
    VideoScene* videoScene;
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
        ASSIGN_OR_RETURN(framebuffer_target_, FrameBufferTarget::Create(),
                         _ << "Failed to create a framebuffer target!");
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
            MP_RETURN_IF_ERROR(GlSetup());
            initialized_ = true;
            // animation_start_time_ = cc->InputTimestamp().Seconds();
        }

        GlTexture input_gl_texture = gpu_helper_.CreateSourceTexture(input_gpu_buffer);
        int dst_width = 1440; // input_gl_texture.width();
        int dst_height = 900; // input_gl_texture.height();
        GlTexture output_gl_texture = gpu_helper_.CreateDestinationTexture(dst_width, dst_height);

        // Set the destination texture as the color buffer. Then, clear both the
        // color and the depth buffers for the render target.
        framebuffer_target_->SetColorbuffer(dst_width, dst_height, output_gl_texture.target(), output_gl_texture.name());
        framebuffer_target_->Clear();

        MP_RETURN_IF_ERROR(GlBind());
        // Run core program.
        MP_RETURN_IF_ERROR(GlRender(cc,
                                    input_gl_texture, 
                                    output_gl_texture, 
                                    cc->InputTimestamp().Seconds()));

        MP_RETURN_IF_ERROR(GlCleanup());
        
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
    framebuffer_target_.reset();
  });
}

absl::Status GlIkOpenglCalculator::GlSetup() {
    // Load joints
    for(int i = 0; i < 10; ++i) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        joints1.push_back(glm::vec3(0, r, 0));
    }
    std::cout << "joint1 size: " << joints1.size() << std::endl;

    joints2.push_back(glm::vec3(0, 0.0f, 0));
    joints2.push_back(glm::vec3(0, 1.0f, 0));
    joints2.push_back(glm::vec3(2.0f, 2.0f, 0));

    // Load our model object
    // Target target(5.0f, 3.0f, 0);
    // Target target2(2, 0, 0);
    // Target target3(1, 1, 0);
    target = new Target(5.0f, 3.0f, 0);

    chain1 = new Chain(joints1, target);
    chain2 = new Chain(joints2, target);
    chain2->please_constrain = true;

    camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));

    videoScene = new VideoScene();
    videoScene->Setup();

    return absl::OkStatus();
}

absl::Status GlIkOpenglCalculator::GlBind() {
    // Setup some OpenGL options
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_MULTISAMPLE);
    
    return absl::OkStatus();
}

absl::Status GlIkOpenglCalculator::GlRender(CalculatorContext* cc, 
                                            const GlTexture& src, 
                                            const GlTexture& dst,
                                            double timestamp) {
    // make sure we clear the framebuffer's content
    int src_width = dst.width();
    int src_height = dst.height();
    glm::mat4 projection = glm::perspective(camera->Zoom, (float)src_width/(float)src_height, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();
    if (count_ > 500) {
        count_ = 0;
        glm::vec3 pos = target->position;
        pos.x = rand() % 3 + 1;
        target->position = pos;
    } 
    count_++;

    chain1->Solve();
    chain2->Solve();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    framebuffer_target_->Bind();
    glViewport(0, 0, src_width, src_height);
    target->Render(view, projection);
    chain1->Render(view, projection);
    chain2->Render(view, projection);

    framebuffer_target_->Unbind();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    framebuffer_target_->Bind();
    glViewport(0, 0, src_width, src_height);
    videoScene->Draw(src.target(), src.name());

    framebuffer_target_->Unbind();    
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    return absl::OkStatus();
}

absl::Status GlIkOpenglCalculator::GlCleanup() {
    // cleanup
    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);
    // glDisable(GL_BLEND);
    glFlush();
    return absl::OkStatus();    
}

absl::Status GlIkOpenglCalculator::GlTeardown() {
    // ourShader->tearDown();
    return absl::OkStatus();
}

}