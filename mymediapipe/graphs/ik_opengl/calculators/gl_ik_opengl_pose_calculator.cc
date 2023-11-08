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
static constexpr char kLandmarksTag[] = "LANDMARKS";
static constexpr char kNormLandmarksTag[] = "NORM_LANDMARKS";
static constexpr char kWorldLandmarksTag[] = "WORLD_LANDMARKS";

class GlIkOpenglPoseCalculator : public CalculatorBase {
public:
    GlIkOpenglPoseCalculator() : initialized_(false), count_(0) {}
    GlIkOpenglPoseCalculator(const GlIkOpenglPoseCalculator&) = delete;
    GlIkOpenglPoseCalculator& operator=(const GlIkOpenglPoseCalculator&) = delete;
    ~GlIkOpenglPoseCalculator();

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
    Target* targetLeftArm;
    Target* targetRightArm;
    Target* targetLeftForeArm;
    Target* targetRightForeArm;
    Target* targetLeftShoulder;
    Target* targetRighShoulder;
    Target* targetLeftHip;
    Target* targetRighHip;
    Target* targetLeftKnee;
    Target* targetRighKnee;
    Target* targetLeftAnkle;
    Target* targetRighAnkle;
    VideoScene* videoScene;
    // std::vector<Target*> targets;
};

REGISTER_CALCULATOR(GlIkOpenglPoseCalculator);

// A calculator that renders a visual effect for multiple faces.
//
// Inputs:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer containing input image.
// Output:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer with a visual effect being rendered for multiple faces.
// static
absl::Status GlIkOpenglPoseCalculator::GetContract(CalculatorContract* cc) {
    MP_RETURN_IF_ERROR(GlCalculatorHelper::UpdateContract(cc))
        << "Failed to update contract for the GPU helper!";

    cc->Inputs().Tag(kImageGpuTag).Set<GpuBuffer>();
    cc->Outputs().Tag(kImageGpuTag).Set<GpuBuffer>();

    if (cc->Inputs().HasTag(kWorldLandmarksTag)) {
        cc->Inputs().Tag(kWorldLandmarksTag).Set<LandmarkList>();
    }

    if (cc->Inputs().HasTag(kLandmarksTag)) {
        cc->Inputs().Tag(kLandmarksTag).Set<NormalizedLandmarkList>();
    }
    // Currently we pass GL context information and other stuff as external
    // inputs, which are handled by the helper.
    return GlCalculatorHelper::UpdateContract(cc);
}

absl::Status GlIkOpenglPoseCalculator::Open(CalculatorContext* cc) {
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

absl::Status GlIkOpenglPoseCalculator::Process(CalculatorContext* cc) {
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

absl::Status GlIkOpenglPoseCalculator::Close(CalculatorContext* cc) {
    return RunInGlContext([this]() -> absl::Status { 
        // return GlTeardown(); 
        return absl::OkStatus();
    });
}

GlIkOpenglPoseCalculator::~GlIkOpenglPoseCalculator() {
  RunInGlContext([this] {
    framebuffer_target_.reset();
  });
}

absl::Status GlIkOpenglPoseCalculator::GlSetup() {
    // Load joints
    // for(int i = 0; i < 10; ++i) {
    //     float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    //     joints1.push_back(glm::vec3(0, r, 0));
    // }
    // std::cout << "joint1 size: " << joints1.size() << std::endl;
    joints1.push_back(glm::normalize(glm::vec3(-3.414665, 7.343246, 0.004669)));
    joints1.push_back(glm::normalize(glm::vec3(0.000000, 7.189681, 0.000000)));
    joints1.push_back(glm::normalize(glm::vec3(0.000000, 24.202721, 0.000000)));
    joints1.push_back(glm::normalize(glm::vec3(0.000000, 21.595646, 0.000000)));

    joints2.push_back(glm::normalize(glm::vec3(3.414665, 7.343371, -0.017399)));
    joints2.push_back(glm::normalize(glm::vec3(0.000000, 7.189681, 0.000000)));
    joints2.push_back(glm::normalize(glm::vec3(0.000000, 24.203573, 0.000000)));
    joints2.push_back(glm::normalize(glm::vec3(0.000000, 21.595100, 0.000000)));
    // joints2.push_back(glm::vec3(0.25f, 0.24f, 0.3));

    // Load our model object
    // Target target(5.0f, 3.0f, 0);
    // Target target2(2, 0, 0);
    // Target target3(1, 1, 0);
    // target = new Target(0.0f, 0.0f, 0.0f);
    targetLeftArm = new Target(0.0f, 0.0f, 0.0f);
    targetRightArm = new Target(0.0f, 0.0f, 0.0f);
    targetLeftForeArm = new Target(0.0f, 0.0f, 0.0f);
    targetRightForeArm = new Target(0.0f, 0.0f, 0.0f);
    targetLeftShoulder = new Target(0.0f, 0.0f, 0.0f);
    targetRighShoulder = new Target(0.0f, 0.0f, 0.0f);
    targetLeftHip = new Target(0.0f, 0.0f, 0.0f);
    targetRighHip = new Target(0.0f, 0.0f, 0.0f);
    targetLeftKnee = new Target(0.0f, 0.0f, 0.0f);
    targetRighKnee = new Target(0.0f, 0.0f, 0.0f);
    targetLeftAnkle = new Target(0.0f, 0.0f, 0.0f);
    targetRighAnkle = new Target(0.0f, 0.0f, 0.0f);

    chain1 = new Chain(joints1, targetLeftArm);
    chain2 = new Chain(joints2, targetLeftArm);
    chain2->please_constrain = true;

    camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));

    videoScene = new VideoScene();
    videoScene->Setup();

    return absl::OkStatus();
}

absl::Status GlIkOpenglPoseCalculator::GlBind() {
    // Setup some OpenGL options
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_MULTISAMPLE);
    
    return absl::OkStatus();
}

absl::Status GlIkOpenglPoseCalculator::GlRender(CalculatorContext* cc, 
                                            const GlTexture& src, 
                                            const GlTexture& dst,
                                            double timestamp) {

    if (cc->Inputs().HasTag(kWorldLandmarksTag) &&
        cc->Inputs().Tag(kWorldLandmarksTag).IsEmpty()) {
        return absl::OkStatus();
    }
    if (cc->Inputs().HasTag(kLandmarksTag) &&
        cc->Inputs().Tag(kLandmarksTag).IsEmpty()) {
        return absl::OkStatus();
    }

    // make sure we clear the framebuffer's content
    int src_width = dst.width();
    int src_height = dst.height();
    glm::mat4 projection = glm::perspective(camera->Zoom, (float)src_width/(float)src_height, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();

    const auto& landmarks_3d = cc->Inputs().Tag(kWorldLandmarksTag).Get<LandmarkList>();
    const auto& landmarks_2d = cc->Inputs().Tag(kLandmarksTag).Get<NormalizedLandmarkList>();
    // if (count_ > 500) {
    //     count_ = 0;
    //     glm::vec3 pos = target->position;
    //     pos.x = rand() % 3 + 1;
    //     target->position = pos;
    // } 
    // count_++;
    targetLeftArm->position  = glm::vec3(landmarks_3d.landmark(16).x(), landmarks_3d.landmark(16).y(), landmarks_3d.landmark(16).z() * -1.0);
    targetRightArm->position = glm::vec3(landmarks_3d.landmark(15).x(), landmarks_3d.landmark(15).y(), landmarks_3d.landmark(15).z() * -1.0);
    targetLeftForeArm->position  = glm::vec3(landmarks_3d.landmark(14).x(), landmarks_3d.landmark(14).y(), landmarks_3d.landmark(14).z() * -1.0);
    targetRightForeArm->position = glm::vec3(landmarks_3d.landmark(13).x(), landmarks_3d.landmark(13).y(), landmarks_3d.landmark(13).z() * -1.0);
    targetLeftShoulder->position  = glm::vec3(landmarks_3d.landmark(12).x(), landmarks_3d.landmark(12).y(), landmarks_3d.landmark(12).z() * -1.0);
    targetRighShoulder->position = glm::vec3(landmarks_3d.landmark(11).x(), landmarks_3d.landmark(11).y(), landmarks_3d.landmark(11).z() * -1.0);
    targetLeftHip->position = glm::vec3(landmarks_3d.landmark(24).x(), landmarks_3d.landmark(24).y(), landmarks_3d.landmark(24).z() * -1.0);
    targetRighHip->position = glm::vec3(landmarks_3d.landmark(23).x(), landmarks_3d.landmark(23).y(), landmarks_3d.landmark(23).z() * -1.0);
    targetLeftKnee->position = glm::vec3(landmarks_3d.landmark(26).x(), landmarks_3d.landmark(26).y(), landmarks_3d.landmark(26).z() * -1.0);
    targetRighKnee->position = glm::vec3(landmarks_3d.landmark(25).x(), landmarks_3d.landmark(25).y(), landmarks_3d.landmark(25).z() * -1.0);
    targetLeftAnkle->position = glm::vec3(landmarks_3d.landmark(28).x(), landmarks_3d.landmark(28).y(), landmarks_3d.landmark(28).z() * -1.0);
    targetRighAnkle->position = glm::vec3(landmarks_3d.landmark(27).x(), landmarks_3d.landmark(27).y(), landmarks_3d.landmark(27).z() * -1.0);

    chain1->Solve();
    chain2->Solve();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    framebuffer_target_->Bind();
    glViewport(0, 0, src_width, src_height);
    targetLeftArm->Render(view, projection);
    targetRightArm->Render(view, projection);
    targetLeftForeArm->Render(view, projection);
    targetRightForeArm->Render(view, projection);
    targetLeftShoulder->Render(view, projection);
    targetRighShoulder->Render(view, projection);
    targetLeftHip->Render(view, projection);
    targetRighHip->Render(view, projection);
    targetLeftKnee->Render(view, projection);
    targetRighKnee->Render(view, projection);
    targetLeftAnkle->Render(view, projection);
    targetRighAnkle->Render(view, projection);
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

absl::Status GlIkOpenglPoseCalculator::GlCleanup() {
    // cleanup
    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);
    // glDisable(GL_BLEND);
    glFlush();
    return absl::OkStatus();    
}

absl::Status GlIkOpenglPoseCalculator::GlTeardown() {
    // ourShader->tearDown();
    return absl::OkStatus();
}

}