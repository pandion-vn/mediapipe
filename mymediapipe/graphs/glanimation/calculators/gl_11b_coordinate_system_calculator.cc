#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "mediapipe/framework/formats/matrix_data.pb.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/modules/face_geometry/libs/procrustes_solver.h"
#include "mediapipe/modules/face_geometry/protos/environment.pb.h"
#include "mediapipe/modules/face_geometry/protos/face_geometry.pb.h"
#include "Eigen/Core"

#include "lib/stb_image.h"
#include "lib/video_scene.h"
#include "lib/mesh.h"

namespace mediapipe {

static constexpr char kImageGpuTag[] = "IMAGE_GPU";
static constexpr char kEnvironmentTag[] = "ENVIRONMENT";
static constexpr char kLandmarksTag[] = "LANDMARKS";
static constexpr char kNormLandmarksTag[] = "NORM_LANDMARKS";
static constexpr char kMultiFaceGeometryTag[] = "MULTI_FACE_GEOMETRY";

enum { ATTRIB_VERTEX, ATTRIB_TEXTURE_COORDS, NUM_ATTRIBUTES };
enum { M_TEXTURE0, M_TEXTURE1, M_TEXTURE2, M_TEXTURE3, NUM_TEXTURES};

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

struct PerspectiveCameraFrustum {
    // NOTE: all arguments must be validated prior to calling this constructor.
    PerspectiveCameraFrustum(const face_geometry::PerspectiveCamera& perspective_camera,
                             int frame_width, int frame_height) {
        static constexpr float kDegreesToRadians = 3.14159265358979323846f / 180.f;

        const float height_at_near =
            2.f * perspective_camera.near() *
            std::tan(0.5f * kDegreesToRadians *
                    perspective_camera.vertical_fov_degrees());

        const float width_at_near = frame_width * height_at_near / frame_height;

        left = -0.5f * width_at_near;
        right = 0.5f * width_at_near;
        bottom = -0.5f * height_at_near;
        top = 0.5f * height_at_near;
        near = perspective_camera.near();
        far = perspective_camera.far();
    }

    float left;
    float right;
    float bottom;
    float top;
    float near;
    float far;
};

void ConvertLandmarkListToEigenMatrix(
    const NormalizedLandmarkList& landmark_list,
    Eigen::Matrix3Xf& eigen_matrix) {
    eigen_matrix = Eigen::Matrix3Xf(3, landmark_list.landmark_size());
    for (int i = 0; i < landmark_list.landmark_size(); ++i) {
        const auto& landmark = landmark_list.landmark(i);
        eigen_matrix(0, i) = landmark.x();
        eigen_matrix(1, i) = landmark.y();
        eigen_matrix(2, i) = landmark.z();
    }
}

void ProjectXY(const PerspectiveCameraFrustum& pcf,
               Eigen::Matrix3Xf& landmarks,
               const face_geometry::Environment& environment) {
    float x_scale = pcf.right - pcf.left;
    float y_scale = pcf.top - pcf.bottom;
    float x_translation = pcf.left;
    float y_translation = pcf.bottom;

    if (/*origin_point_location_*/environment.origin_point_location() == face_geometry::OriginPointLocation::TOP_LEFT_CORNER) {
        landmarks.row(1) = 1.f - landmarks.row(1).array();
    }

    landmarks = landmarks.array().colwise() * Eigen::Array3f(x_scale, y_scale, x_scale);
    landmarks.colwise() += Eigen::Vector3f(x_translation, y_translation, 0.f);
}

// https://learnopengl.com/Getting-started/Coordinate-Systems
class Gl11bCoordinateSystemCalculator : public CalculatorBase {
public:
    Gl11bCoordinateSystemCalculator() : initialized_(false) {}
    Gl11bCoordinateSystemCalculator(const Gl11bCoordinateSystemCalculator&) = delete;
    Gl11bCoordinateSystemCalculator& operator=(const Gl11bCoordinateSystemCalculator&) = delete;
    ~Gl11bCoordinateSystemCalculator();

    static absl::Status GetContract(CalculatorContract* cc);
    absl::Status Open(CalculatorContext* cc) override;
    absl::Status Process(CalculatorContext* cc) override;
    absl::Status Close(CalculatorContext* cc) override;

    absl::Status GlSetup();
    absl::Status GlBind();
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp);
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
    VideoScene *videoScene;
    GLuint program_ = 0;
    GLint texture0_, texture1_, texture2_;
    GLint model_, view_, projection_;
    GLuint VBO, VAO, EBO, TextureBO[2];
    std::vector<face_geometry::FaceGeometry> empty_multi_face_geometry;
    std::vector<face_geometry::FaceGeometry> multi_face_geometry;
    face_geometry::Environment environment_;
    NormalizedLandmarkList landmarks;

    std::array<float, 16> CreatePerspectiveMatrix(float aspect_ratio) const;
    static std::array<float, 16> Create4x4IdentityMatrix() {
        return {1.f, 0.f, 0.f, 0.f,  //
                0.f, 1.f, 0.f, 0.f,  //
                0.f, 0.f, 1.f, 0.f,  //
                0.f, 0.f, 0.f, 1.f};
    }

    std::array<float, 16> Convert4x4MatrixDataToArrayFormat(const MatrixData& matrix_data);
    absl::Status Convert(const NormalizedLandmarkList& screen_landmark_list,  //
                         const PerspectiveCameraFrustum& pcf,                 //
                         LandmarkList& metric_landmark_list,                  //
                         Eigen::Matrix4f& pose_transform_mat);

    static void ChangeHandedness(Eigen::Matrix3Xf& landmarks) {
        landmarks.row(2) *= -1.f;
    }

    static void UnprojectXY(const PerspectiveCameraFrustum& pcf,
                          Eigen::Matrix3Xf& landmarks) {
        landmarks.row(0) =
            landmarks.row(0).cwiseProduct(landmarks.row(2)) / pcf.near;
        landmarks.row(1) =
            landmarks.row(1).cwiseProduct(landmarks.row(2)) / pcf.near;
    }

    static void MoveAndRescaleZ(const PerspectiveCameraFrustum& pcf,
                                float depth_offset, float scale,
                                Eigen::Matrix3Xf& landmarks) {
        landmarks.row(2) =
            (landmarks.array().row(2) - depth_offset + pcf.near) / scale;
    }

    absl::StatusOr<float> EstimateScale(Eigen::Matrix3Xf& landmarks) const;

    static void ConvertEigenMatrixToLandmarkList(
        const Eigen::Matrix3Xf& eigen_matrix, LandmarkList& landmark_list) {
        
        landmark_list.Clear();
        for (int i = 0; i < eigen_matrix.cols(); ++i) {
            auto& landmark = *landmark_list.add_landmark();
            landmark.set_x(eigen_matrix(0, i));
            landmark.set_y(eigen_matrix(1, i));
            landmark.set_z(eigen_matrix(2, i));
        }
    }

    Eigen::Matrix3Xf canonical_metric_landmarks_;
    Eigen::VectorXf landmark_weights_;
    std::unique_ptr<face_geometry::ProcrustesSolver> procrustes_solver_;
    std::vector<Vertex> vertices_;
};

REGISTER_CALCULATOR(Gl11bCoordinateSystemCalculator);

// A calculator that renders a visual effect for multiple faces.
//
// Inputs:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer containing input image.
// Output:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer with a visual effect being rendered for multiple faces.
// static
absl::Status Gl11bCoordinateSystemCalculator::GetContract(CalculatorContract* cc) {
    MP_RETURN_IF_ERROR(GlCalculatorHelper::UpdateContract(cc))
        << "Failed to update contract for the GPU helper!";

    cc->Inputs().Tag(kImageGpuTag).Set<GpuBuffer>();
    
    cc->InputSidePackets()
            .Tag(kEnvironmentTag)
            .Set<face_geometry::Environment>();

    if (cc->Inputs().HasTag(kNormLandmarksTag)) {
        cc->Inputs().Tag(kNormLandmarksTag).Set<NormalizedLandmarkList>();
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

absl::Status Gl11bCoordinateSystemCalculator::Open(CalculatorContext* cc) {
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

absl::Status Gl11bCoordinateSystemCalculator::Process(CalculatorContext* cc) {
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
        int dst_width = input_gl_texture.width();
        int dst_height = input_gl_texture.height();
        GlTexture output_gl_texture = gpu_helper_.CreateDestinationTexture(dst_width, dst_height);

        bool visualize_depth = true;// options_.visualize_landmark_depth();
        float z_min = 0.f;
        float z_max = 0.f;
        if (cc->Inputs().HasTag(kNormLandmarksTag)) {
            landmarks = cc->Inputs().Tag(kNormLandmarksTag).Get<NormalizedLandmarkList>();
            if (visualize_depth) {
                GetMinMaxZ<NormalizedLandmarkList, NormalizedLandmark>(landmarks, &z_min, &z_max);
            }

            for (int i = 0; i < landmarks.landmark_size(); ++i) {
                const NormalizedLandmark& landmark = landmarks.landmark(i);
                std::cout << "landmark: " << landmark.x() << " " << landmark.y() << " " << landmark.z() << std::endl;
            }
        }
        // get face geometry
        // multi_face_geometry = cc->Inputs().Tag(kMultiFaceGeometryTag).IsEmpty()
        //                                         ? empty_multi_face_geometry
        //                                         : cc->Inputs()
        //                                             .Tag(kMultiFaceGeometryTag)
        //                                             .Get<std::vector<face_geometry::FaceGeometry>>();

        // Set the destination texture as the color buffer. Then, clear both the
        // color and the depth buffers for the render target.
        framebuffer_target_->SetColorbuffer(dst_width, dst_height, output_gl_texture.target(), output_gl_texture.name());
        framebuffer_target_->Clear();

        MP_RETURN_IF_ERROR(GlBind());
        // Run core program.
        MP_RETURN_IF_ERROR(GlRender(input_gl_texture, 
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

absl::Status Gl11bCoordinateSystemCalculator::Close(CalculatorContext* cc) {
    return RunInGlContext([this]() -> absl::Status { 
        return GlTeardown(); 
    });
}

Gl11bCoordinateSystemCalculator::~Gl11bCoordinateSystemCalculator() {
  RunInGlContext([this] {
    framebuffer_target_.reset();
  });
}

absl::Status Gl11bCoordinateSystemCalculator::GlSetup() {

    // Load vertex and fragment shaders
    const GLint attr_location[NUM_ATTRIBUTES] = {
        ATTRIB_VERTEX,
        // ATTRIB_COLOR,
        ATTRIB_TEXTURE_COORDS,
    };

    const GLchar* attr_name[NUM_ATTRIBUTES] = {
        "position",
        "tex_coord",
    };

    const GLchar* vert_src = R"(
        attribute vec3 position;
        // attribute vec3 color;
        attribute vec2 tex_coord;

        // uniform float gScale;
        // uniform mat4 transform;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        // varying vec3 ourColor;
        varying vec2 ourTexCoord;

        void main()
        {
            gl_Position = projection * view * model * vec4(position, 1.0);
            // ourColor = color;
            ourTexCoord = tex_coord;
        }
    )";

    const GLchar* frag_src = R"(
        precision mediump float;
        // varying vec3 ourColor;
        varying vec2 ourTexCoord;
        uniform sampler2D texture0;
        // uniform sampler2D texture1;
        // uniform sampler2D texture2;

        void main()
        {
            // gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
            // gl_FragColor = vec4(ourColor, 1.0);
            // gl_FragColor = texture2D(texture0, ourTexCoord) * vec4(ourColor, 1.0);
            gl_FragColor = texture2D(texture0, ourTexCoord);
            // vec4 color = mix(texture2D(texture0, ourTexCoord), texture2D(texture1, ourTexCoord), 0.4);
            // vec4 color1 = mix(color, texture2D(texture2, ourTexCoord), 0.2);
            // gl_FragColor = color1;
        } 
    )";

    // shader program
    GlhCreateProgram(vert_src, frag_src, NUM_ATTRIBUTES,
                    (const GLchar**)&attr_name[0], attr_location, &program_);
    RET_CHECK(program_) << "Problem initializing the program.";
    
    model_ = glGetUniformLocation(program_, "model");
    RET_CHECK_NE(model_, -1) << "Failed to find `model` uniform!";

    view_ = glGetUniformLocation(program_, "view");
    RET_CHECK_NE(view_, -1) << "Failed to find `view` uniform!";

    projection_ = glGetUniformLocation(program_, "projection");
    RET_CHECK_NE(projection_, -1) << "Failed to find `projection` uniform!";

    texture0_ = glGetUniformLocation(program_, "texture0");
    RET_CHECK_NE(texture0_, -1) << "Failed to find `texture` uniform!";

    // generate texture
    glGenTextures(2, TextureBO);
    glBindTexture(GL_TEXTURE_2D, TextureBO[0]);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("mymediapipe/assets/opengl/question-mark.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Loaded texture" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, TextureBO[1]);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width1, height1, nrChannels1;
    unsigned char *data1 = stbi_load("mymediapipe/assets/opengl/awesomeface.png", &width1, &height1, &nrChannels1, 0);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Loaded texture" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data1);
    glBindTexture(GL_TEXTURE_2D, 0);


    videoScene = new VideoScene();
    videoScene->Setup();

    procrustes_solver_ = face_geometry::CreateFloatPrecisionProcrustesSolver();

    std::cout << "DONE setup" << std::endl;
    return absl::OkStatus();
}

absl::Status Gl11bCoordinateSystemCalculator::GlBind() {

    // glm::mat4 transform = glm::mat4(1.0);
    // transform = glm::translate(transform, glm::vec3(0.3, -0.3, 0.0));

    // Initial potition of dot
    // GLfloat vertices[] = {
    //     // positions      
    //     //  0.5f,  0.5f, 0.0f,  // top right
    //     //  0.5f, -0.5f, 0.0f,  // bottom right
    //     // -0.5f, -0.5f, 0.0f,  // bottom left
    //     // -0.5f,  0.5f, 0.0f,  // top left

    //     // cube positions
    //     -0.5f, -0.5f, -0.5f, 
    //     0.5f, -0.5f, -0.5f,  
    //     0.5f,  0.5f, -0.5f,  
    //     0.5f,  0.5f, -0.5f,  
    //     -0.5f,  0.5f, -0.5f, 
    //     -0.5f, -0.5f, -0.5f, 

    //     -0.5f, -0.5f,  0.5f, 
    //     0.5f, -0.5f,  0.5f,  
    //     0.5f,  0.5f,  0.5f,  
    //     0.5f,  0.5f,  0.5f,  
    //     -0.5f,  0.5f,  0.5f, 
    //     -0.5f, -0.5f,  0.5f, 

    //     -0.5f,  0.5f,  0.5f, 
    //     -0.5f,  0.5f, -0.5f, 
    //     -0.5f, -0.5f, -0.5f, 
    //     -0.5f, -0.5f, -0.5f, 
    //     -0.5f, -0.5f,  0.5f, 
    //     -0.5f,  0.5f,  0.5f, 

    //     0.5f,  0.5f,  0.5f,  
    //     0.5f,  0.5f, -0.5f,  
    //     0.5f, -0.5f, -0.5f,  
    //     0.5f, -0.5f, -0.5f,  
    //     0.5f, -0.5f,  0.5f,  
    //     0.5f,  0.5f,  0.5f,  

    //     -0.5f, -0.5f, -0.5f, 
    //     0.5f, -0.5f, -0.5f,  
    //     0.5f, -0.5f,  0.5f,  
    //     0.5f, -0.5f,  0.5f,  
    //     -0.5f, -0.5f,  0.5f, 
    //     -0.5f, -0.5f, -0.5f, 

    //     -0.5f,  0.5f, -0.5f, 
    //     0.5f,  0.5f, -0.5f,  
    //     0.5f,  0.5f,  0.5f,  
    //     0.5f,  0.5f,  0.5f,  
    //     -0.5f,  0.5f,  0.5f, 
    //     -0.5f,  0.5f, -0.5f, 
    // };

    // GLfloat texture_coords[] = {
    //     // texture coords square
    //     // 1.0f, 1.0f,     // top right
    //     // 1.0f, 0.0f,     // bottom right
    //     // 0.0f, 0.0f,     // bottom left
    //     // 0.0f, 1.0f,     // top left
    //     // 1.0f, 0.0f, // lower-right corner
    //     // 0.0f, 0.0f, // lower-left corner
    //     // 0.5f, 1.0f, // top-center corner

    //     // cube coords
    //     0.0f, 0.0f,
    //     1.0f, 0.0f,
    //     1.0f, 1.0f,
    //     1.0f, 1.0f,
    //     0.0f, 1.0f,
    //     0.0f, 0.0f,
    //     0.0f, 0.0f,
    //     1.0f, 0.0f,
    //     1.0f, 1.0f,
    //     1.0f, 1.0f,
    //     0.0f, 1.0f,
    //     0.0f, 0.0f,
    //     1.0f, 0.0f,
    //     1.0f, 1.0f,
    //     0.0f, 1.0f,
    //     0.0f, 1.0f,
    //     0.0f, 0.0f,
    //     1.0f, 0.0f,
    //     1.0f, 0.0f,
    //     1.0f, 1.0f,
    //     0.0f, 1.0f,
    //     0.0f, 1.0f,
    //     0.0f, 0.0f,
    //     1.0f, 0.0f,
    //     0.0f, 1.0f,
    //     1.0f, 1.0f,
    //     1.0f, 0.0f,
    //     1.0f, 0.0f,
    //     0.0f, 0.0f,
    //     0.0f, 1.0f,
    //     0.0f, 1.0f,
    //     1.0f, 1.0f,
    //     1.0f, 0.0f,
    //     1.0f, 0.0f,
    //     0.0f, 0.0f,
    //     0.0f, 1.0f
    // };

    // GLuint indices[] = {
    //     0, 1, 3,        // first triangle
    //     1, 2, 3,        // second triangle
    // };

    for (int i = 0; i < landmarks.landmark_size(); ++i) {
        Vertex vertex;
        const NormalizedLandmark& landmark = landmarks.landmark(i);
        vertex.Position = glm::vec3(landmark.x(), landmark.y(), landmark.z());
        // std::cout << "landmark: " << landmark.x() << " " << landmark.y() << " " << landmark.z() << std::endl;
        vertices_.push_back(vertex);
    }
    
    glUseProgram(program_);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate Vertext Buffer Object for vertex
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Bind data
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_DYNAMIC_DRAW);
    // vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Generate Vertex Buffer Object for texture coords
    // glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coords), texture_coords, GL_STATIC_DRAW);
    // texture coord attribute
    // glVertexAttribPointer(ATTRIB_TEXTURE_COORDS, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(float), nullptr);
    // glEnableVertexAttribArray(ATTRIB_TEXTURE_COORDS);

    // Generate indices bufer object for element array
    // glGenBuffers(1, &EBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glUniform1i(texture0_, M_TEXTURE0);
    glEnable(GL_BLEND);

    return absl::OkStatus();
}

absl::Status Gl11bCoordinateSystemCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                                       double timestamp) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    int src_width = src.width();
    int src_height = src.height();

    // Create a perspective matrix using the frame aspect ratio.
    std::array<float, 16> perspective_matrix = CreatePerspectiveMatrix(
        /*aspect_ratio*/ static_cast<float>(src_width) / src_height);

    std::cout << "perspective_matrix: ";
    for (std::size_t i = 0; i < perspective_matrix.size(); ++i)
        std::cout << perspective_matrix.data()[i] << ' ';
    std::cout << std::endl;

    // Estimate Keypoints Geometry from Landmarks
        int frame_width = src_width;
        int frame_height = src_height;
        const auto& perspective_camera_ = environment_.perspective_camera();
        // Create a perspective camera frustum to be shared for geometry estimateion per keypoint
        PerspectiveCameraFrustum pcf(perspective_camera_, frame_width, frame_height);

        // From this point, the meaning of "landmarks" as "screen pose landmarks"

        // Convert the screen landmarks into the metric landmarks and get the pose
        // transformation matrix.
        const auto& screen_face_landmarks = landmarks;
        LandmarkList metric_face_landmarks;
        Eigen::Matrix4f pose_transform_mat;
        Convert(screen_face_landmarks, pcf, metric_face_landmarks, pose_transform_mat);
        for (int i = 0; i < metric_face_landmarks.landmark_size(); ++i) {
            const auto& landmark = metric_face_landmarks.landmark(i);
            std::cout << "geometry landmark: " << landmark.x() << " " << landmark.y() << " " << landmark.z() << std::endl;
        }

        // Pack geometry data for this pose.
        // FaceGeometry face_geometry;

        // Populate the face pose transformation matrix.
        // mediapipe::MatrixDataProtoFromMatrix(pose_transform_mat, face_geometry.mutable_pose_transform_matrix());


    // std::array<float, 16> face_pose_transform_matrice = 
    //         Convert4x4MatrixDataToArrayFormat(multi_face_geometry[0].pose_transform_matrix());

    videoScene->Draw(*framebuffer_target_, src.target(), src.name(), src_width, src_height);
    glUseProgram(program_);

    // create transformations
    glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // model = glm::rotate(model, (float) timestamp * glm::radians(20.0f), glm::vec3(0.5f, 1.0f, 0.0f));  
    view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(63.0f), (float)src_width / (float)src_height, 0.1f, 10000.0f);

    std::cout << "projection glm: " << glm::to_string(projection) << std::endl;

    // glm::mat4 model = glm::make_mat4(face_pose_transform_matrice.data());
    model = glm::scale(model, glm::vec3(.5f, .5f, .5f));	// it's a bit too big for our scene, so scale it down
    // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    // model = glm::rotate(model, (float) timestamp * glm::radians(20.0f), glm::vec3(0.5f, 1.0f, 0.0f));  
    // pass them to the shaders (3 different ways)
    // glUniformMatrix4fv(model_, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_, 1, GL_FALSE, &view[0][0]);
    // glUniformMatrix4fv(projection_, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(model_, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projection_, 1, GL_FALSE, perspective_matrix.data());


    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    framebuffer_target_->Bind();
    // glUseProgram(program_);
    // drawring 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureBO[0]);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(vertices_.size()));
    // glDrawArrays(GL_POINTS, 0, static_cast<unsigned int>(vertices_.size()));
    // std::cout << "glDrawArrays texture" << std::endl;
    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    framebuffer_target_->Unbind();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // unbind VAO
    glBindVertexArray(0);

    // unbind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return absl::OkStatus();    
}

absl::Status Gl11bCoordinateSystemCalculator::GlCleanup() {
    // cleanup
    glDisableVertexAttribArray(ATTRIB_VERTEX);
    // glDisableVertexAttribArray(ATTRIB_COLOR);
    glDisableVertexAttribArray(ATTRIB_TEXTURE_COORDS);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    return absl::OkStatus();    
}

absl::Status Gl11bCoordinateSystemCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}

std::array<float, 16> Gl11bCoordinateSystemCalculator::CreatePerspectiveMatrix(float aspect_ratio) const {
    static constexpr float kDegreesToRadians = M_PI / 180.f;

    std::array<float, 16> perspective_matrix;
    perspective_matrix.fill(0.f);

    const auto& env_camera = environment_.perspective_camera();
    // Standard perspective projection matrix calculations.
    const float f = 1.0f / std::tan(kDegreesToRadians *
                                    env_camera.vertical_fov_degrees() / 2.f);

    const float denom = 1.0f / (env_camera.near() - env_camera.far());
    perspective_matrix[0] = f / aspect_ratio;
    perspective_matrix[5] = f;
    perspective_matrix[10] = (env_camera.near() + env_camera.far()) * denom;
    perspective_matrix[11] = -1.f;
    perspective_matrix[14] = 2.f * env_camera.far() * env_camera.near() * denom;

    // If the environment's origin point location is in the top left corner,
    // then skip additional flip along Y-axis is required to render correctly.
    if (environment_.origin_point_location() ==
        face_geometry::OriginPointLocation::TOP_LEFT_CORNER) {
        perspective_matrix[5] *= -1.f;
    }

    return perspective_matrix;
}

std::array<float, 16> Gl11bCoordinateSystemCalculator::Convert4x4MatrixDataToArrayFormat(const MatrixData& matrix_data) {
    // RET_CHECK(matrix_data.rows() == 4 &&  //
    //             matrix_data.cols() == 4 &&  //
    //             matrix_data.packed_data_size() == 16)
    //     << "The matrix data must define a 4x4 matrix!";

    std::array<float, 16> matrix_array;
    for (int i = 0; i < 16; i++) {
        matrix_array[i] = matrix_data.packed_data(i);
    }

    // Matrix array must be in the OpenGL-friendly column-major order. If
    // `matrix_data` is in the row-major order, then transpose.
    if (matrix_data.layout() == MatrixData::ROW_MAJOR) {
        std::swap(matrix_array[1], matrix_array[4]);
        std::swap(matrix_array[2], matrix_array[8]);
        std::swap(matrix_array[3], matrix_array[12]);
        std::swap(matrix_array[6], matrix_array[9]);
        std::swap(matrix_array[7], matrix_array[13]);
        std::swap(matrix_array[11], matrix_array[14]);
    }

    return matrix_array;
}

absl::Status Gl11bCoordinateSystemCalculator::Convert(const NormalizedLandmarkList& screen_landmark_list,  //
                                                      const PerspectiveCameraFrustum& pcf,                 //
                                                      LandmarkList& metric_landmark_list,                  //
                                                      Eigen::Matrix4f& pose_transform_mat) {
    // (1) Project X- and Y- screen landmark coordinates at the Z near plane.
    // (2) Estimate a canonical-to-runtime landmark set scale by running the
    //     Procrustes solver using the screen runtime landmarks.
    // (3) Use the canonical-to-runtime scale from (2) to unproject the screen
    //     landmarks.
    // (4) Estimate a canonical-to-runtime landmark set scale by running the
    //     Procrustes solver using the intermediate runtime landmarks.
    // (5) Use the product of the scale factors from (2) and (4) to unproject
    //     the screen landmarks the second time.
    // (6) Multiply each of the metric landmarks by the inverse pose
    //     transformation matrix to align the runtime metric face landmarks with
    //     the canonical metric face landmarks.
    Eigen::Matrix3Xf screen_landmarks;
    ConvertLandmarkListToEigenMatrix(screen_landmark_list, screen_landmarks);

    ProjectXY(pcf, screen_landmarks, environment_);
    const float depth_offset = screen_landmarks.row(2).mean();
    // 1st iteration
    Eigen::Matrix3Xf intermediate_landmarks(screen_landmarks);
    ChangeHandedness(intermediate_landmarks);
    ASSIGN_OR_RETURN(const float first_iteration_scale, EstimateScale(intermediate_landmarks),
                     _ << "Failed to estimate first iteration scale!");
    // 2nd iteration
    intermediate_landmarks = screen_landmarks;
    MoveAndRescaleZ(pcf, depth_offset, first_iteration_scale,
                    intermediate_landmarks);
    UnprojectXY(pcf, intermediate_landmarks);
    ChangeHandedness(intermediate_landmarks);
    ASSIGN_OR_RETURN(const float second_iteration_scale, EstimateScale(intermediate_landmarks),
                     _ << "Failed to estimate second iteration scale!");

    // Use the total scale to unproject the screen landmarks.
    const float total_scale = first_iteration_scale * second_iteration_scale;
    MoveAndRescaleZ(pcf, depth_offset, total_scale, screen_landmarks);
    UnprojectXY(pcf, screen_landmarks);
    ChangeHandedness(screen_landmarks);

    // At this point, screen landmarks are converted into metric landmarks.
    Eigen::Matrix3Xf& metric_landmarks = screen_landmarks;
    // MP_RETURN_IF_ERROR(procrustes_solver_->SolveWeightedOrthogonalProblem(
    //                                             canonical_metric_landmarks_, 
    //                                             metric_landmarks, 
    //                                             landmark_weights_,
    //                                             pose_transform_mat))
    //     << "Failed to estimate pose transform matrix!";
    
    // Multiply each of the metric landmarks by the inverse pose
    // transformation matrix to align the runtime metric face landmarks with
    // the canonical metric face landmarks.
    // metric_landmarks = (pose_transform_mat.inverse() *
    //                     metric_landmarks.colwise().homogeneous())
    //                     .topRows(3);

    ConvertEigenMatrixToLandmarkList(metric_landmarks, metric_landmark_list);
    return absl::OkStatus();
}

absl::StatusOr<float> Gl11bCoordinateSystemCalculator::EstimateScale(Eigen::Matrix3Xf& landmarks) const {
    Eigen::Matrix4f transform_mat;
    // MP_RETURN_IF_ERROR(procrustes_solver_->SolveWeightedOrthogonalProblem(
    //     canonical_metric_landmarks_, landmarks, landmark_weights_,
    //     transform_mat))
    //     << "Failed to estimate canonical-to-runtime landmark set transform!";

    return transform_mat.col(0).norm();
  }

} // mediapipe