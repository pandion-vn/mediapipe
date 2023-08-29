#ifndef SANDBOX_H
#define SANDBOX_H

#include "common.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "neon_renderer.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"
#include "mediapipe/util/resource_util.h"

class Sandbox {
private:
    static std::atomic<bool>    dataReady;
    static bool                 stopCapture;
    std::mutex                  mutexCamBuffer;
    std::thread                 threadCam;
    Texture2D                   camTexture;
    cv::Mat                     camBuffer;

public:
    Sandbox(unsigned int width, unsigned int height);
    ~Sandbox();

    unsigned int        width, height;
    bool                keys[1024];
    int32_t             frames;

    // static attrs
    static mediapipe::CalculatorGraph                       graph;
    static mediapipe::GlCalculatorHelper                    gpuHelper;
    static mediapipe::StatusOrPoller                        poller_status;
    static GLFWwindow*                                      glfwWindow;
    static cv::VideoCapture                                 camCapture;
    static SpriteRenderer                                   *spriteRenderer;
    static NeonRenderer                                     *neonRenderer;

    // static methods
    static mediapipe::Status InitialMPPGraph();
    static int InitOpenGL();
    static int InitCamCapture();
    static int InitEffekseer(unsigned int viewportW, unsigned int viewportH);
    static void ConfigureOpenGL(unsigned int viewportW, unsigned int viewportH);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    // initialize state (load all shaders/textures/levels)
    mediapipe::Status Init();
    

    // sandbox app loop
    void ProcessInput(float dt);
    void Update(float dt);
    void UpdateCamera(float dt);
    void UpdateCamera1(cv::Mat &camBuffer, float dt);
    void UpdateEffekseer(float dt);
    void Render(float dt);

    // reset ...
    void Reset();

    // camera thread
    void stop();
    void join();
    void DoCamCapture();

    // camera texture
};

#endif
