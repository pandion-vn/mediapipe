#ifndef SANDBOX_H
#define SANDBOX_H

#include "common.h"

class Sandbox {
private:
public:
    Sandbox(unsigned int width, unsigned int height);
    ~Sandbox();

    unsigned int        width, height;
    bool                keys[1024];
    int32_t             frames;

    // static attrs
    static mediapipe::CalculatorGraph                       graph;
    // static mediapipe::GlCalculatorHelper                    gpuHelper;
    static std::unique_ptr<mediapipe::OutputStreamPoller>   poller;
    static GLFWwindow*                                      glfwWindow;
    static cv::VideoCapture                                 camCapture;
    // static SpriteRenderer                                   *spriteRenderer;

    // static methods
    static mediapipe::Status InitialMPPGraph();
    static int InitOpenGL();
    static int InitCamCapture();
    static int InitEffekseer(unsigned int viewportW, unsigned int viewportH);
    static void ConfigureOpenGL(unsigned int viewportW, unsigned int viewportH);

    // initialize state (load all shaders/textures/levels)
    mediapipe::Status Init();
    

    // sandbox app loop
    void ProcessInput(float dt);
    void Update(float dt);
    void UpdateCamera(float dt);
    void UpdateEffekseer(float dt);
    void Render(float dt);

    // reset ...
    void Reset();

    // camera thread
    void Stop();
    void Join();
    void DoCamCapture();

    // camera texture
};

#endif
