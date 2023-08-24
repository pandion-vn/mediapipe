#include "../include/sandbox.h"
// #include <experimental/filesystem>

constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";
constexpr char kWindowName[] = "MediaPipe";

ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");
ABSL_FLAG(std::string, input_video_path, "",
          "Full path of video to load. "
          "If not provided, attempt to use a webcam.");
ABSL_FLAG(std::string, output_video_path, "",
          "Full path of where to save result (.mp4 only). "
          "If not provided, show result in a window.");

Sandbox::Sandbox(unsigned int width, unsigned int height) 
    : keys(), width(width), height(height), frames(0) {
}

Sandbox::~Sandbox() {
    //cam_capture.release();
    //delete Renderer;
    // delete gpu_helper;
}

mediapipe::Status Sandbox::Init() {
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
}

int Sandbox::InitOpenGL() {
}

void Sandbox::ConfigureOpenGL(unsigned int viewport_w, unsigned int viewport_h) {
}

void Sandbox::UpdateCamera(float dt) {
}

void Sandbox::ProcessInput(float dt) {
    // empty now
}

void Sandbox::Render(float dt) {
}

void Sandbox::Reset() {
}

void Sandbox::Stop() {
}

void Sandbox::Join() {
}

void Sandbox::DoCamCapture() {
    // while (!stopCapture) {
    // }
}