#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"
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

#include "lib/stb_image.h"
#include "lib/shader.h"
#include "lib/camera.h"
#include "lib/model.h"
#include "lib/video_scene.h"
#include "lib/model_scene_a.h"
#include "lib/model_line.h"
#include "lib/animation.h"
#include "lib/animator.h"
// #include "lib/util.h"
#include "lib/my_pose.h"
// #include "lib/kalidokit.h"

namespace mediapipe {

static constexpr char kImageGpuTag[] = "IMAGE_GPU";
static constexpr char kEnvironmentTag[] = "ENVIRONMENT";
static constexpr char kLandmarksTag[] = "LANDMARKS";
static constexpr char kNormLandmarksTag[] = "NORM_LANDMARKS";
static constexpr char kWorldLandmarksTag[] = "WORLD_LANDMARKS";
static constexpr char kMultiFaceGeometryTag[] = "MULTI_FACE_GEOMETRY";

enum { ATTRIB_VERTEX, ATTRIB_NORMAL, ATTRIB_TEXTURE_COORDS, LIGHTING_NUM_ATTRIBUTES };

template <class LandmarkListType, class LandmarkType>
inline void GetMinMaxZ(const LandmarkListType& landmarks, float* z_min, float* z_max) {
    *z_min = std::numeric_limits<float>::max();
    *z_max = std::numeric_limits<float>::min();
    for (int i = 0; i < landmarks.landmark_size(); ++i) {
        const LandmarkType& landmark = landmarks.landmark(i);
        *z_min = std::min(landmark.z(), *z_min);
        *z_max = std::max(landmark.z(), *z_max);
    }
}

// https://learnopengl.com/Model-Loading/Mesh
// https://learnopengl.com/Model-Loading/Model
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
class Gl18aCameraSkeletalAnimationCalculator : public CalculatorBase {
public:
    Gl18aCameraSkeletalAnimationCalculator() : initialized_(false) {}
    Gl18aCameraSkeletalAnimationCalculator(const Gl18aCameraSkeletalAnimationCalculator&) = delete;
    Gl18aCameraSkeletalAnimationCalculator& operator=(const Gl18aCameraSkeletalAnimationCalculator&) = delete;
    ~Gl18aCameraSkeletalAnimationCalculator();

    static absl::Status GetContract(CalculatorContract* cc);
    absl::Status Open(CalculatorContext* cc) override;
    absl::Status Process(CalculatorContext* cc) override;
    absl::Status Close(CalculatorContext* cc) override;

    absl::Status GlSetup();
    absl::Status GlBind();
    absl::Status GlRender(CalculatorContext* cc, const GlTexture& src, const GlTexture& dst, double timestamp);
    absl::Status GlCleanup();
    absl::Status GlTeardown();

    // You can override this method to compute the size of the destination
    // texture. By default, it will take the same size as the source texture.
    virtual void GetOutputDimensions(int src_width, int src_height,
                                    int* dst_width, int* dst_height) {
        *dst_width = src_width;
        *dst_height = src_height;
    }

    // Override this to output a different type of buffer.
    virtual GpuBufferFormat GetOutputFormat() { return GpuBufferFormat::kBGRA32; }

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

    GlCalculatorHelper gpu_helper_;
    bool initialized_;
    double animation_start_time_;

private:
    // GLuint VBO[3], cubeVAO, lightCubeVAO, EBO, TextureBO[2];
    // GLuint diffuseMapTexture;
    // GLuint VBO[2], VAO;
    VideoScene *videoScene;
    ModelSceneA *modelScene;
    ModelLine *modelLine;
    int counting;
    face_geometry::Environment environment_;
    LandmarkList prev_landmarks;
    Animation *animation;
};

REGISTER_CALCULATOR(Gl18aCameraSkeletalAnimationCalculator);

// A calculator that renders a visual effect for multiple faces.
//
// Inputs:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer containing input image.
// Output:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer with a visual effect being rendered for multiple faces.
// static
absl::Status Gl18aCameraSkeletalAnimationCalculator::GetContract(CalculatorContract* cc) {
    MP_RETURN_IF_ERROR(GlCalculatorHelper::UpdateContract(cc))
        << "Failed to update contract for the GPU helper!";

    cc->Inputs().Tag(kImageGpuTag).Set<GpuBuffer>();
    
    cc->InputSidePackets()
            .Tag(kEnvironmentTag)
            .Set<face_geometry::Environment>();

    if (cc->Inputs().HasTag(kWorldLandmarksTag)) {
        cc->Inputs().Tag(kWorldLandmarksTag).Set<LandmarkList>();
    }

    if (cc->Inputs().HasTag(kLandmarksTag)) {
        cc->Inputs().Tag(kLandmarksTag).Set<NormalizedLandmarkList>();
    }
    
    if (cc->Inputs().HasTag(kMultiFaceGeometryTag)) {
        cc->Inputs()
                .Tag(kMultiFaceGeometryTag)
                .Set<std::vector<face_geometry::FaceGeometry>>();
    }

    cc->Outputs().Tag(kImageGpuTag).Set<GpuBuffer>();
    // Currently we pass GL context information and other stuff as external
    // inputs, which are handled by the helper.
    return GlCalculatorHelper::UpdateContract(cc);
}

absl::Status Gl18aCameraSkeletalAnimationCalculator::Open(CalculatorContext* cc) {
    // Inform the framework that we always output at the same timestamp
    // as we receive a packet at.
    cc->SetOffset(mediapipe::TimestampDiff(0));

    // Let the helper access the GL context information.
    MP_RETURN_IF_ERROR(gpu_helper_.Open(cc)) 
        << "Failed to open the GPU helper!";
    return RunInGlContext([this, cc]() -> absl::Status {
        environment_ = cc->InputSidePackets()
                            .Tag(kEnvironmentTag)
                            .Get<face_geometry::Environment>();

        ASSIGN_OR_RETURN(framebuffer_target_, FrameBufferTarget::Create(),
                         _ << "Failed to create a framebuffer target!");
        return absl::OkStatus();
    });
}

absl::Status Gl18aCameraSkeletalAnimationCalculator::Process(CalculatorContext* cc) {
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
            animation_start_time_ = cc->InputTimestamp().Seconds();
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

absl::Status Gl18aCameraSkeletalAnimationCalculator::Close(CalculatorContext* cc) {
    return RunInGlContext([this]() -> absl::Status { 
        return GlTeardown(); 
    });
}

Gl18aCameraSkeletalAnimationCalculator::~Gl18aCameraSkeletalAnimationCalculator() {
  RunInGlContext([this] {
    framebuffer_target_.reset();
  });
}

absl::Status Gl18aCameraSkeletalAnimationCalculator::GlSetup() {

    // diffuseMapTexture = loadTexture("mymediapipe/assets/opengl/cube2/Square swirls.png");
    videoScene = new VideoScene();
    videoScene->Setup();

    modelScene = new ModelSceneA();
    modelScene->Setup();

    modelLine = new ModelLine();
    modelLine->Setup();

    counting = 0;

    LOG(INFO) << "DONE setup";
    return absl::OkStatus();
}

absl::Status Gl18aCameraSkeletalAnimationCalculator::GlBind() {    
    glEnable(GL_BLEND);
    // glEnable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);
    return absl::OkStatus();
}

absl::Status Gl18aCameraSkeletalAnimationCalculator::GlRender(CalculatorContext* cc, const GlTexture& src, const GlTexture& dst, double timestamp) {
    // make sure we clear the framebuffer's content
    int src_width = dst.width();
    int src_height = dst.height();
    double deltaTime = timestamp - animation_start_time_;
    // std::cout << "FPS: " << (1.0f / deltaTime) << std::endl;
    animation_start_time_ = timestamp;

    videoScene->Draw(*framebuffer_target_, src.target(), src.name(), src_width, src_height);

    if (cc->Inputs().HasTag(kWorldLandmarksTag) &&
        cc->Inputs().Tag(kWorldLandmarksTag).IsEmpty()) {
        return absl::OkStatus();
    }
    if (cc->Inputs().HasTag(kLandmarksTag) &&
        cc->Inputs().Tag(kLandmarksTag).IsEmpty()) {
        return absl::OkStatus();
    }

    bool visualize_depth = true;// options_.visualize_landmark_depth();
    float z_min = 0.f;
    float z_max = 0.f;
    const auto& landmarks_3d = cc->Inputs().Tag(kWorldLandmarksTag).Get<LandmarkList>();
    const auto& landmarks_2d = cc->Inputs().Tag(kLandmarksTag).Get<NormalizedLandmarkList>();
    if (visualize_depth) {
        GetMinMaxZ<LandmarkList, Landmark>(landmarks_3d, &z_min, &z_max);
    }
    std::cout << "z_min: " << z_min << ", z_max: " << z_max << std::endl;

    // for (int i = 0; i < landmarks.landmark_size(); ++i) {
    //     const Landmark& landmark = landmarks.landmark(i);
    //     std::cout << "landmark (" << i << ") : " << landmark.x() << " " << landmark.y() << " " << landmark.z() << std::endl;
    // }

    // std::cout << std::endl;
    // return absl::OkStatus();

    // prepare landmarks
    std::vector<glm::vec3> lm3d;
    std::vector<glm::vec3> lm2d;
    std::vector<float> lm3d_visibility;
    for (int i = 0; i < landmarks_3d.landmark_size(); ++i) {
        const Landmark& landmark3d = landmarks_3d.landmark(i);
        const NormalizedLandmark& landmark2d = landmarks_2d.landmark(i);
        
        lm3d.push_back(glm::vec3(landmark3d.x() * -1.0, landmark3d.y() * -1.0, landmark3d.z() * -1.0));
        lm3d_visibility.push_back(landmark3d.visibility());
        // lm2d.push_back(glm::vec3(landmark2d.x(), landmark2d.y(), landmark2d.z()));
        lm2d.push_back(glm::vec3(landmark2d.x(), landmark2d.y(), landmark2d.z()));
    }

    auto rotations3d = pose_rotation(lm3d);
    // auto rotations2d = pose_rotation(lm2d);
    // for (int i = 0; i < lm3d.size(); ++i) {
    //     std::cout << "lm3d (" << i << ")" << glm::to_string(lm3d[i]) << std::endl;
    // }

    // for (int i = 0; i < lm2d.size(); ++i) {
    //     std::cout << "lm2d (" << i << ")" << glm::to_string(lm2d[i]) << std::endl;
    // }

    // std::cout << "rot leftarm 3d" << glm::to_string(rotations3d[12]) << std::endl;
    std::cout << "lm3d rightarm[12] " << glm::to_string(rotations3d[12]) << std::endl;
    std::cout << "lm3d rightforearm[14] " << glm::to_string(rotations3d[14]) << std::endl;
    std::cout << "lm3d righthand[16] " << glm::to_string(rotations3d[16]) << std::endl;
    std::cout << "lm3d leftarm[11] " << glm::to_string(rotations3d[11]) << std::endl;
    std::cout << "lm3d leftarm[13] " << glm::to_string(rotations3d[13]) << std::endl;
    std::cout << "lm3d lefarm [15] " << glm::to_string(rotations3d[15]) << std::endl;
    // std::cout << "rot rightarm 3d " << glm::to_string(rotations3d[12]) << std::endl;
    // std::cout << "rot rightarm 2d " << glm::to_string(rotations2d[12]) << std::endl;

    // std::vector<glm::vec3> rots(33);
    // kalidokitSolve(lm3d, lm3d_visibility, lm2d, rots);

    modelScene->Draw(*framebuffer_target_, src_width, src_height, deltaTime, lm3d, rotations3d);
    modelLine->Draw(*framebuffer_target_, src_width, src_height, timestamp, lm3d);

    return absl::OkStatus();
}

absl::Status Gl18aCameraSkeletalAnimationCalculator::GlCleanup() {
    // cleanup
    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);
    // glDisable(GL_BLEND);
    glFlush();
    return absl::OkStatus();    
}

absl::Status Gl18aCameraSkeletalAnimationCalculator::GlTeardown() {
    // ourShader->tearDown();
    return absl::OkStatus();
}

} // mediapipe