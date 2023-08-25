#include "../include/sandbox.h"
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
GLFWwindow*                             Sandbox::glfwWindow;
cv::VideoCapture                        Sandbox::camCapture;
std::atomic<bool>                       Sandbox::dataReady;
bool                                    Sandbox::stopCapture;
SpriteRenderer*                         Sandbox::spriteRenderer;
NeonRenderer*                           Sandbox::neonRenderer;

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


    LOG(INFO) << "Start running the calculator graph.";
    threadCam = std::thread(&Sandbox::DoCamCapture, this);
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
    mediapipe::CalculatorGraph graph;
    MP_RETURN_IF_ERROR(graph.Initialize(config));
    graph.StartRun({});
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    // glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    // glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    // glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    // glfwWindowHint(GLFW_ALPHA_BITS, 8);
    // glfwWindowHint(GLFW_DEPTH_BITS, 24);
    // glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    // enable anti-alising 4x with GLFW
    glfwWindowHint(GLFW_SAMPLES, 0);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
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
    
#ifdef __APPLE__
#else // Not APPLE
//     glewExperimental = true;
//     GLenum err = glewInit();
//     if (GLEW_OK != err) {
//         fprintf(stderr, " GLEW INIT ERROR !!!  . \n");
//         std::cerr << "Error initialization GLEW: " << glewGetErrorString(err) << std::endl;
//         exit(EXIT_FAILURE);
//     }
#endif
    return 0;
}

void Sandbox::ConfigureOpenGL(unsigned int viewportW, unsigned int viewportH) {
    glfwSetKeyCallback(glfwWindow, Sandbox::KeyCallback);
    glfwSetFramebufferSizeCallback(glfwWindow, Sandbox::FramebufferSizeCallback);

    // OpenGL configuration
    // --------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0, 1.0);
    glDepthMask(true);
    glClearDepth(1.0f);
    glEnable(GL_LINE_SMOOTH);
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
    stopCapture = false;
    dataReady = false;
    return 0;
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
    while (!stopCapture) {
        // LOG(INFO) << "Do Camera Capture";
        camCapture >> camBufferTmp;
        if (camBufferTmp.empty()) {
            // LOG(INFO) << "Ignore empty frames";
            continue;
        }

        cv::Mat camBuff;	
        cv::cvtColor(camBufferTmp, camBuff, cv::COLOR_BGR2RGBA);
        cv::flip(camBuff, camBuff, /*flipcode=HORIZONTAL*/ 1);

        std::lock_guard<std::mutex> lk (mutexCamBuffer);
        camBuff.copyTo(camBuffer);
        dataReady = true;
    }
}