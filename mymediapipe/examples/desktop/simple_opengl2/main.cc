#include <cstdlib>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "include/common.h"
#include "include/sandbox.h"

Sandbox* sandboxApp;

mediapipe::Status RunMPPGraph() {
    mediapipe::Status status = Sandbox::InitialMPPGraph();
    if (!status.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << status.message();
        return mediapipe::Status(mediapipe::StatusCode::kInternal, "Failed to run the graph");
    } else {
        LOG(INFO) << "Success!";
    }

    srand(time(NULL));
    Sandbox::InitOpenGL();
    Sandbox::ConfigureOpenGL(1280, 720);
    Sandbox::InitCamCapture();
    sandboxApp = new Sandbox(1280, 720);
    sandboxApp->Init();
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

    glfwDestroyWindow(Sandbox::glfwWindow);
    glfwTerminate();
    LOG(INFO) << "Shutting down.";
    MP_RETURN_IF_ERROR(Sandbox::graph.CloseInputStream(kInputStream));
    return Sandbox::graph.WaitUntilDone();
    // return mediapipe::OkStatus();
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