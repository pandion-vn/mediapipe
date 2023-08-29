#include "../include/sandbox_gpu.h"

// #include <experimental/filesystem>

ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");
ABSL_FLAG(std::string, input_video_path, "",
          "Full path of video to load. "
          "If not provided, attempt to use a webcam.");
ABSL_FLAG(std::string, output_video_path, "",
          "Full path of where to save result (.mp4 only). "
          "If not provided, show result in a window.");

// Instantiate static variables
mediapipe::CalculatorGraph              Sandbox::graph;
mediapipe::GlCalculatorHelper           Sandbox::gpu_helper;
GLFWwindow*                             Sandbox::glfwWindow;
cv::VideoCapture                        Sandbox::camCapture;
std::atomic<bool>                       Sandbox::dataReady;
bool                                    Sandbox::stopCapture;
SpriteRenderer*                         Sandbox::spriteRenderer;
NeonRenderer*                           Sandbox::neonRenderer;
// std::unique_ptr<mediapipe::OutputStreamPoller>              Sandbox::poller;
mediapipe::StatusOrPoller               Sandbox::poller_status;

Sandbox::Sandbox(unsigned int width, unsigned int height) 
    : keys(), width(width), height(height), frames(0) {
}

Sandbox::~Sandbox() {
    //cam_capture.release();
    //delete Renderer;
    //delete gpu_helper;
}

mediapipe::Status Sandbox::Init() {
    // load shaders
    ResourceManager::LoadShader("mymediapipe/assets/shaders/sprite.vs", 
                                "mymediapipe/assets/shaders/sprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("mymediapipe/assets/shaders/neon_swing.vs", 
                                "mymediapipe/assets/shaders/neon_swing.fs", nullptr, "neon_swing");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->width), static_cast<float>(this->height), 
                                      0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    ResourceManager::GetShader("neon_swing").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("neon_swing").Use().SetInteger("viewfinder", 1);
    ResourceManager::GetShader("neon_swing").Use().SetInteger("background", 2);
    ResourceManager::GetShader("neon_swing").SetMatrix4("projection", projection);


    // set render-specific controls
    Shader shaderSprite = ResourceManager::GetShader("sprite");
    spriteRenderer = new SpriteRenderer(shaderSprite);

    Shader neonSprite = ResourceManager::GetShader("neon_swing");
    neonRenderer = new NeonRenderer(neonSprite, this->width, this->height);

    // load textures
    // ResourceManager::LoadTexture("mymediapipe/assets/textures/play_your_hand.jpg", false, "background");
    ResourceManager::LoadTexture("mymediapipe/assets/textures/background.jpg", false, "background");

    LOG(INFO) << "Start running the thread camera capture.";
    Sandbox::threadCam = std::thread(&Sandbox::DoCamCapture, this);

    return mediapipe::OkStatus();
}

mediapipe::Status Sandbox::InitialMPPGraph() {
    std::string calculator_graph_config_contents;
    MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
        absl::GetFlag(FLAGS_calculator_graph_config_file), &calculator_graph_config_contents));
    LOG(INFO) << "Get calculator graph config contents: "
              << calculator_graph_config_contents;
    mediapipe::CalculatorGraphConfig config =
            mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(calculator_graph_config_contents);

    LOG(INFO) << "Initialize the calculator graph.";
    // mediapipe::CalculatorGraph graph;
    MP_RETURN_IF_ERROR(graph.Initialize(config));

    LOG(INFO) << "Initialize the GPU.";
    ASSIGN_OR_RETURN(auto gpu_resources, mediapipe::GpuResources::Create());
    MP_RETURN_IF_ERROR(graph.SetGpuResources(std::move(gpu_resources)));
    // mediapipe::GlCalculatorHelper gpu_helper;
    gpu_helper.InitializeForTest(graph.GetGpuResources().get());
    
    LOG(INFO) << "Start running the calculator graph.";
    poller_status = graph.AddOutputStreamPoller(kOutputStream);
    // poller_status.ok();
    // poller = std::move(poller_status.value());
    // poller.reset(
        // new mediapipe::OutputStreamPoller(graph.AddOutputStreamPoller(kOutputStream).value()));
    
    MP_RETURN_IF_ERROR(graph.StartRun({}));
    // graph.StartRun({});
    return mediapipe::OkStatus();
}

int Sandbox::InitOpenGL() {
    if (!glfwInit()) {
        LOG(INFO) << "GLFW Initial error";
        return EXIT_FAILURE;
    }
    LOG(INFO) << "GLFW VERSION : " << glfwGetVersionString();

    // GLFWmonitor *primary = glfwGetPrimaryMonitor();
    // const GLFWvidmode *mode = glfwGetVideoMode(primary);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // enable anti-alising 4x with GLFW
    // glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RESIZABLE, false);
    
    glfwWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, kWindowName, nullptr, nullptr);
    if (glfwWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(glfwWindow);
    // glfwSwapInterval(1);

    /* start GLEW extension handler */
	// glewExperimental = GL_TRUE;
    // GLenum err = glewInit();
    // if (GLEW_OK != err) {
    //     fprintf(stderr, " GLEW INIT ERROR !!!  . \n");
    //     std::cerr << "Error initialization GLEW: " << glewGetErrorString(err) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

	/* get version info */
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version supported: " << glGetString(GL_VERSION) << std::endl;
    return 0;
}

void Sandbox::ConfigureOpenGL(unsigned int viewportW, unsigned int viewportH) {
    glfwSetKeyCallback(glfwWindow, Sandbox::KeyCallback);
    glfwSetFramebufferSizeCallback(glfwWindow, Sandbox::FramebufferSizeCallback);

    // OpenGL configuration
    // --------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // glDepthRange(0.0, 1.0);
    glDepthMask(true);
    glClearDepth(1.0f);
    // glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glViewport(0, 0, viewportW, viewportH);
}

void Sandbox::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // empty now
}

void Sandbox::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // empty now
    glViewport(0, 0, width, height);
}

int Sandbox::InitCamCapture() {
    // if (camCapture.open(gst, cv::CAP_GSTREAMER)) {
    if (camCapture.open(0)) {
        LOG(INFO) << "Init Opencv Camera Success";
    } else {
        LOG(INFO) << "File To Open Camera";
        camCapture.release();
        return 1;
    }
    if (!camCapture.isOpened()) {
        LOG(INFO) << "Cam capture cannot open";
        return 1;
    }
#if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
    camCapture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    camCapture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    camCapture.set(cv::CAP_PROP_FPS, 30);
#endif
    stopCapture = false;
    dataReady = false;
    return 0;
}

void Sandbox::UpdateCamera1(cv::Mat &camBuffer, float dt) {
    ResourceManager::LoadTextureFromMat(camTexture, camBuffer, true);
}

void Sandbox::UpdateCamera(float dt) {
    std::unique_lock<std::mutex> lck(mutexCamBuffer, std::defer_lock);
    if (dataReady && lck.try_lock()) {
        // LOG(INFO) << "Do ResourceManager::LoadTextureFromMat camBuffer";
        ResourceManager::LoadTextureFromMat(camTexture, camBuffer, true);
        dataReady = false;
    }

    // if (HasHuman) {
    //     cameraView = 0.08f;
    //     HasHuman = false;
    // }

    // if (HasOpenHand) {
    //     openEffect = 0.08f;
    //     HasOpenHand = false;
    // }

    // if (cameraView > 0) {
    //     cameraView -= dt;
    //     // if (camView > 0.1) {
    //     neonRenderer->Phase = 2;
    //     // } else {
    //         // neonRenderer->Phase = 1;
    //     // }
    // } else {
    //     neonRenderer->Phase = 0;
    // }
}

void Sandbox::ProcessInput(float dt) {
    // empty now
}

void Sandbox::Update(float dt)
{
    // LOG(INFO) << "Do Sandbox::Update";
    UpdateCamera(dt);
    // UpdateEffekseer(dt);

    // Frames++;
}

void Sandbox::Render(float dt) {

    // LOG(INFO) << "Do Sandbox::Render";
    // Texture2D background = ResourceManager::GetTexture("background");
    // spriteRenderer->DrawSprite(background, 
    //                            glm::vec2(0.0f, 0.0f), glm::vec2(this->width, this->height), 
    //                            45.0f, glm::vec3(0.0f, 1.0f, 0.0f), dt);

    spriteRenderer->DrawSprite(camTexture, 
                               glm::vec2(0.0f, 0.0f), glm::vec2(this->width, this->height), 
                               0.0f, glm::vec3(1.0f), dt);
    // neonRenderer->DrawSprite(camTexture,
    //                          glm::vec2(0.0f, 0.0f), glm::vec2(this->width, this->height), 
    //                          0.0f, glm::vec3(1.0f), dt);
}

void Sandbox::Reset() {
    // empty now
}

void Sandbox::stop() {
    this->stopCapture = true;
}

void Sandbox::join() {
    this->threadCam.join();
}

void Sandbox::DoCamCapture() {
    cv::Mat camera_frame_raw;

    while (!stopCapture) {
        // LOG(INFO) << "Do Camera Capture";
        camCapture >> camera_frame_raw;
        if (camera_frame_raw.empty()) {
            // LOG(INFO) << "Ignore empty frames";
            continue;
        }

        cv::Mat camera_frame;	
        cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
        cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);
        // LOG(INFO) << "camera_frame size" << camera_frame.size() << " channels: " << camera_frame.channels();;

        // Wrap Mat into an ImageFrame.
        auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGB, camera_frame.cols, camera_frame.rows,
            mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
        camera_frame.copyTo(input_frame_mat);

        // Send image packet into the graph.
        size_t frame_timestamp_us =
            (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
        // LOG(INFO) << "Timestamp : " << frame_timestamp_us;
        gpu_helper.RunInGlContext([&input_frame, &frame_timestamp_us, &graph,
                                    &gpu_helper]() -> absl::Status {
            // Convert ImageFrame to GpuBuffer.
            auto texture = gpu_helper.CreateSourceTexture(*input_frame.get());
            auto gpu_frame = texture.GetFrame<mediapipe::GpuBuffer>();
            glFlush();
            texture.Release();
            // Send GPU image packet into the graph.
            MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
                kInputStream, mediapipe::Adopt(gpu_frame.release())
                                    .At(mediapipe::Timestamp(frame_timestamp_us))));
            return absl::OkStatus();
        });
        // graph.AddPacketToInputStream(
        //     kInputStream, mediapipe::Adopt(input_frame.release())
        //                     .At(mediapipe::Timestamp(frame_timestamp_us)));

        // Get the graph result packet, or stop if that fails.
        mediapipe::Packet packet;
        if (!poller_status->Next(&packet)) continue;
        // auto& output_frame = packet.Get<mediapipe::ImageFrame>();
        std::unique_ptr<mediapipe::ImageFrame> output_frame;
        // Convert GpuBuffer to ImageFrame.
        gpu_helper.RunInGlContext(
            [&packet, &output_frame, &gpu_helper]() -> absl::Status {
                auto& gpu_frame = packet.Get<mediapipe::GpuBuffer>();
                auto texture = gpu_helper.CreateSourceTexture(gpu_frame);
                output_frame = absl::make_unique<mediapipe::ImageFrame>(
                    mediapipe::ImageFormatForGpuBufferFormat(gpu_frame.format()),
                    gpu_frame.width(), gpu_frame.height(),
                    mediapipe::ImageFrame::kGlDefaultAlignmentBoundary);
                gpu_helper.BindFramebuffer(texture);
                const auto info = mediapipe::GlTextureInfoForGpuBufferFormat(
                    gpu_frame.format(), 0, gpu_helper.GetGlVersion());
                glReadPixels(0, 0, texture.width(), texture.height(), info.gl_format,
                            info.gl_type, output_frame->MutablePixelData());
                glFlush();
                texture.Release();
                return absl::OkStatus();
            });

        // Convert back to opencv for display or saving.
        // cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
        cv::Mat output_frame_mat = mediapipe::formats::MatView(output_frame.get());
        if (output_frame_mat.channels() == 4) {
        } else {
            cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2RGBA);
        }
        // LOG(INFO) << "output_frame size" << output_frame_mat.size() << " channels: " << output_frame_mat.channels();;

        // cv::imshow("kWindowName", output_frame_mat);
        std::lock_guard<std::mutex> lk (mutexCamBuffer);
        output_frame_mat.copyTo(camBuffer);
        dataReady = true;
    }
    // Sandbox::graph.CloseAllInputStreams();
}