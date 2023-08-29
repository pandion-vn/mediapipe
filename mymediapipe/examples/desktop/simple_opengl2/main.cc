#include <cstdlib>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "include/common.h"
#include "include/sandbox.h"

Sandbox* sandboxApp;

mediapipe::Status RunMPPGraph() {
    srand(time(NULL));

    Sandbox::InitOpenGL();

    // Debugger FramebufferSize
    int viewport_w, viewport_h;
    float viewport_aspect;
    glfwGetFramebufferSize(Sandbox::glfwWindow, &viewport_w, &viewport_h);
    viewport_aspect = float(viewport_h) / float(viewport_w);
    LOG(INFO) << "viewport_w: " << viewport_w <<  
                 " , viewport_h: " << viewport_h << 
                 " , aspect: " << viewport_aspect;
    
    sandboxApp = new Sandbox(viewport_w, viewport_h);
    Sandbox::ConfigureOpenGL(viewport_w, viewport_h);
    
    
    // deltaTime variables
    // -------------------
    static double limitFPS = 1.0 / 30.0;
    float deltaTime = 0.0f;
    float fps = 0.0f;
    float lastFrame = glfwGetTime(), timer = lastFrame;
    int32_t frames = 0, updates = 0;

    mediapipe::Status status = Sandbox::InitialMPPGraph();
    if (!status.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << status.message();
        return mediapipe::Status(mediapipe::StatusCode::kInternal, "Failed to run the graph");
    } else {
        LOG(INFO) << "Success Initial MPP Graph!";
    }
    sandboxApp->Init();
    Sandbox::InitCamCapture();

    while (!glfwWindowShouldClose(Sandbox::glfwWindow)) {
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
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sandboxApp->Render(currentFrame);

        frames++;
        glfwSwapBuffers(Sandbox::glfwWindow);
        glfwPollEvents();
        if (glfwGetTime() - timer > 1.0) {
            timer++;
            LOG(INFO) << "FPS: " << frames << ", updates:" << updates;
            updates = 0, frames = 0;
        }
    }
    sandboxApp->stop();
    sandboxApp->join();

    // based on camera capture
    // bool grab_frames = true;
    // while (grab_frames && !glfwWindowShouldClose(Sandbox::glfwWindow)) {
    //     float currentFrame = glfwGetTime();
    //     deltaTime = currentFrame - lastFrame;
    //     cv::Mat camera_frame_raw;
    //     fps += deltaTime / limitFPS;
    //     lastFrame = currentFrame;

    //     while (fps >= 1.0) {
    //         Sandbox::camCapture >> camera_frame_raw;
    //         if (camera_frame_raw.empty()) {
    //             LOG(INFO) << "Empty frame, end of video reached.";
    //             break;
    //         }
    //         cv::Mat camera_frame;
    //         cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
    //         cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);

    //         // Wrap Mat into an ImageFrame.
    //         auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
    //             mediapipe::ImageFormat::SRGB, camera_frame.cols, camera_frame.rows,
    //             mediapipe::ImageFrame::kDefaultAlignmentBoundary);
    //         cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
    //         camera_frame.copyTo(input_frame_mat);

    //         // Send image packet into the graph.
    //         size_t frame_timestamp_us =
    //             (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
    //         MP_RETURN_IF_ERROR(Sandbox::graph.AddPacketToInputStream(
    //             kInputStream, mediapipe::Adopt(input_frame.release())
    //                             .At(mediapipe::Timestamp(frame_timestamp_us))));

    //         // Get the graph result packet, or stop if that fails.
    //         mediapipe::Packet packet;
    //         if (!Sandbox::poller->Next(&packet)) break;
    //         auto& output_frame = packet.Get<mediapipe::ImageFrame>();

    //         // Convert back to opencv for display or saving.
    //         cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
    //         // ResourceManager::LoadTextureFromMat(camTexture, camBuffer, true);
    //         // output_frame_mat.copyTo(Sandbox::camBuffer);
    //         sandboxApp->UpdateCamera1(output_frame_mat, currentFrame);
    //         updates++;
    //         fps--;
    //     }

    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //     // sandboxApp->Update(deltaTime);
    //     sandboxApp->Render(currentFrame);
    //     glfwSwapBuffers(Sandbox::glfwWindow);
    //     glfwPollEvents();
    // }

    LOG(INFO) << "Shutting down.";
    MP_RETURN_IF_ERROR(Sandbox::graph.CloseInputStream(kInputStream));
    glfwDestroyWindow(Sandbox::glfwWindow);
    glfwTerminate();
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