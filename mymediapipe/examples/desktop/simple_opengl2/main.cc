#include <cstdlib>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "include/common.h"
#include "include/sandbox.h"

Sandbox* sandboxApp;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


mediapipe::Status RunMPPGraph() {
    mediapipe::Status status = Sandbox::InitialMPPGraph();
    if (!status.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << status.message();
        return mediapipe::Status(mediapipe::StatusCode::kInternal, "Failed to run the graph");
    } else {
        LOG(INFO) << "Success!";
    }
    Sandbox::InitOpenGL();

    // Debugger FramebufferSize
    int viewport_w, viewport_h;
    float viewport_aspect;
    glfwGetFramebufferSize(Sandbox::glfwWindow, &viewport_w, &viewport_h);
    viewport_aspect = float(viewport_h) / float(viewport_w);
    LOG(INFO) << "viewport_w: " << viewport_w <<  
                 " , viewport_h: " << viewport_h << 
                 " , aspect: " << viewport_aspect;

    srand(time(NULL));
    
    sandboxApp = new Sandbox(viewport_w, viewport_h);
    Sandbox::ConfigureOpenGL(viewport_w, viewport_h);
    sandboxApp->Init();
    Sandbox::InitCamCapture();
    // deltaTime variables
    // -------------------
    static double limitFPS = 1.0 / 30.0;
    float deltaTime = 0.0f;
    float fps = 0.0f;
    float lastFrame = glfwGetTime(), timer = lastFrame;
    int32_t frames = 0, updates = 0;

    while (!glfwWindowShouldClose(Sandbox::glfwWindow)) {
        glfwPollEvents();

        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        fps += deltaTime / limitFPS;
        lastFrame = currentFrame;

        while (fps >= 1.0) {
            // manage user input
            // -----------------
            sandboxApp->ProcessInput(deltaTime);

            // update game state
            // -----------------
            sandboxApp->Update(deltaTime);
            updates++;
            fps--;
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        sandboxApp->Render(currentFrame);
        frames++;
        glfwSwapBuffers(Sandbox::glfwWindow);
        if (glfwGetTime() - timer > 1.0) {
            timer++;
            LOG(INFO) << "FPS: " << frames << ", updates:" << updates;
            updates = 0, frames = 0;
        }
    }
    sandboxApp->stop();
    sandboxApp->join();

    glfwDestroyWindow(Sandbox::glfwWindow);
    glfwTerminate();
    LOG(INFO) << "Shutting down.";
    MP_RETURN_IF_ERROR(Sandbox::graph.CloseInputStream(kInputStream));
    // return Sandbox::graph.WaitUntilDone();
    return mediapipe::OkStatus();
}
    
int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    absl::ParseCommandLine(argc, argv);
    mediapipe::Status runStatus = RunMPPGraph();
  
    if (!runStatus.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << runStatus.message();
        return EXIT_FAILURE;
    } else {
        LOG(INFO) << "Success!";
    }
    return EXIT_SUCCESS;
}