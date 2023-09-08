#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "json.hpp"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/statusor.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"

using json = nlohmann::json;

constexpr char kWindowName[] = "Simple Graph GPU Demo";
constexpr char kInputStream[] = "input_video";
constexpr char kInputWidthStream[] = "input_width";
constexpr char kInputHeightStream[] = "input_height";
constexpr char kOutputStream[] = "output_video";

ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");

ABSL_FLAG(std::string, input_side_packets, "",
          "Comma-separated list of key=value pairs specifying side packets "
          "for the CalculatorGraph. All values will be treated as the "
          "string type even if they represent doubles, floats, etc.");

ABSL_FLAG(std::string, input_video_path, "",
          "Full path of video to load. "
          "If not provided, attempt to use a webcam.");
ABSL_FLAG(std::string, output_video_path, "",
          "Full path of where to save result (.mp4 only). "
          "If not provided, show result in a window.");

// Local file output flags.
// Output stream
ABSL_FLAG(std::string, output_stream, "",
          "The output stream to output to the local file in csv format.");
ABSL_FLAG(std::string, output_stream_file, "",
          "The name of the local file to output all packets sent to "
          "the stream specified with --output_stream. ");
ABSL_FLAG(bool, strip_timestamps, false,
          "If true, only the packet contents (without timestamps) will be "
          "written into the local file.");

// Output side packets
ABSL_FLAG(std::string, output_side_packets, "",
          "A CSV of output side packets to output to local file.");
ABSL_FLAG(std::string, output_side_packets_file, "",
          "The name of the local file to output all side packets specified "
          "with --output_side_packets. ");




cv::Mat GetRgb(const std::string& path) {
    cv::Mat bgr = cv::imread(path);
    cv::Mat rgb;
    cv::cvtColor(bgr, rgb, cv::COLOR_BGR2RGB);
    return rgb;
}

mediapipe::ImageFormat::Format GetImageFormat(int image_channels) {
  if (image_channels == 4) {
    return mediapipe::ImageFormat::SRGBA;
  } else if (image_channels == 3) {
    return mediapipe::ImageFormat::SRGB;
  } else if (image_channels == 1) {
    return mediapipe::ImageFormat::GRAY8;
  }
  LOG(FATAL) << "Unsupported input image channles: " << image_channels;
}

mediapipe::Packet MakeImageFramePacket(cv::Mat input) {
    mediapipe::ImageFrame input_image(GetImageFormat(input.channels()), input.cols,
                                      input.rows, input.step, input.data, [](uint8_t*) {});
    return mediapipe::MakePacket<mediapipe::ImageFrame>(std::move(input_image));
}

absl::Status OutputStreamToLocalFile(mediapipe::OutputStreamPoller& poller) {
    std::ofstream file;
    file.open(absl::GetFlag(FLAGS_output_stream_file));
    mediapipe::Packet packet;
    while (poller.Next(&packet)) {
        std::string output_data;
        if (!absl::GetFlag(FLAGS_strip_timestamps)) {
            absl::StrAppend(&output_data, packet.Timestamp().Value(), ",");
        }
        absl::StrAppend(&output_data, packet.Get<std::string>(), "\n");
        file << output_data;
    }
    file.close();
    return absl::OkStatus();
}

absl::Status OutputStreamToConsole(mediapipe::OutputStreamPoller& poller) {
    mediapipe::Packet packet;
    while (poller.Next(&packet)) {
        std::string output_data;
        if (!absl::GetFlag(FLAGS_strip_timestamps)) {
            absl::StrAppend(&output_data, packet.Timestamp().Value(), ",");
        }
        absl::StrAppend(&output_data, packet.Get<std::string>(), "\n");
        LOG(INFO) << output_data;
    }
    
    return absl::OkStatus();
}

absl::Status OutputSidePacketsToLocalFile(mediapipe::CalculatorGraph& graph) {
    std::ofstream file;
    file.open(absl::GetFlag(FLAGS_output_side_packets_file));
    std::vector<std::string> side_packet_names =
        absl::StrSplit(absl::GetFlag(FLAGS_output_side_packets), ',');
    for (const std::string& side_packet_name : side_packet_names) {
        ASSIGN_OR_RETURN(auto status_or_packet,
                         graph.GetOutputSidePacket(side_packet_name));
        file << absl::StrCat(side_packet_name, ":",
                             status_or_packet.Get<std::string>(), "\n");
    }
    file.close();
    return absl::OkStatus();
}

absl::Status OutputSidePacketsToConsole(mediapipe::CalculatorGraph& graph) {
    std::vector<std::string> side_packet_names =
        absl::StrSplit(absl::GetFlag(FLAGS_output_side_packets), ',');
    for (const std::string& side_packet_name : side_packet_names) {
        ASSIGN_OR_RETURN(auto status_or_packet,
                         graph.GetOutputSidePacket(side_packet_name));
        LOG(INFO) << absl::StrCat(side_packet_name, ":",
                                  status_or_packet.Get<std::string>(), "\n");
    }
    return absl::OkStatus();
}

absl::Status OutputSidePackets(mediapipe::CalculatorGraph& graph) {
    if (!absl::GetFlag(FLAGS_output_side_packets_file).empty()) {
        return OutputSidePacketsToLocalFile(graph);
    } else {
        return OutputSidePacketsToConsole(graph);
    }

    return absl::OkStatus();
}

absl::Status InputStreamSample(mediapipe::CalculatorGraph &graph) {
    // Give 10 input packets that contains the same string "Hello World!".
    for (int i = 0; i < 10; ++i) {
        MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
            "in", mediapipe::MakePacket<std::string>("Hello World!").At(mediapipe::Timestamp(i))));
    }
    MP_RETURN_IF_ERROR(graph.CloseInputStream("in"));
    return absl::OkStatus();
}

absl::StatusOr<std::map<std::string, mediapipe::Packet>> InitialInputSidePackets() {
    std::map<std::string, mediapipe::Packet> input_side_packets;
    if (!absl::GetFlag(FLAGS_input_side_packets).empty()) {
        std::vector<std::string> kv_pairs =
                absl::StrSplit(absl::GetFlag(FLAGS_input_side_packets), ',');
        for (const std::string& kv_pair : kv_pairs) {
            std::vector<std::string> name_and_value = absl::StrSplit(kv_pair, '=');
            RET_CHECK(name_and_value.size() == 2);
            RET_CHECK(!mediapipe::ContainsKey(input_side_packets, name_and_value[0]));
            
            if (name_and_value[0].find("_image") != std::string::npos) {
                // LOG(INFO) << "Input side packet is image: " << name_and_value[0];
                // cv::Mat cv_mat = GetRgb(name_and_value[1]);
                // input_side_packets[name_and_value[0]] = MakeImageFramePacket(cv_mat);
            } else {
                input_side_packets[name_and_value[0]] =
                    mediapipe::MakePacket<std::string>(name_and_value[1]);
            }
        }
    }
    return input_side_packets;
}

std::unique_ptr<mediapipe::ImageFrame> WrapMatToImageFrame(cv::Mat& camera_frame) {
    auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGBA, camera_frame.cols, camera_frame.rows,
            mediapipe::ImageFrame::kGlDefaultAlignmentBoundary);
        cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
        camera_frame.copyTo(input_frame_mat);
    return input_frame;
}

absl::StatusOr<std::unique_ptr<mediapipe::GpuBuffer>> 
    ImageFrameToGpuBuffer(std::unique_ptr<mediapipe::ImageFrame> &input_frame, 
                          mediapipe::GlCalculatorHelper &gpu_helper) {
    std::unique_ptr<mediapipe::GpuBuffer> gpu_buffer;

    MP_RETURN_IF_ERROR(
        gpu_helper.RunInGlContext([&input_frame, &gpu_helper, &gpu_buffer]() -> absl::Status {
            // Convert ImageFrame to GpuBuffer.
            auto texture = gpu_helper.CreateSourceTexture(*input_frame.get());
            gpu_buffer = texture.GetFrame<mediapipe::GpuBuffer>();
            glFlush();
            texture.Release();

            // Send GPU image packet into the graph.
            // MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
            //     kInputStream, mediapipe::Adopt(gpu_frame.release())
            //                         .At(mediapipe::Timestamp(frame_timestamp_us))));
            return absl::OkStatus();
        })
    );
    return gpu_buffer;
}

absl::StatusOr<mediapipe::Packet> CreateRGBPacket(const std::string& path, 
                                                  mediapipe::GlCalculatorHelper &gpu_helper) {
    cv::Mat cv_mat = GetRgb(path);
    auto box_frame = WrapMatToImageFrame(cv_mat);
    ASSIGN_OR_RETURN(auto gpu_frame, ImageFrameToGpuBuffer(box_frame, gpu_helper));
    return mediapipe::Adopt(gpu_frame.release());
    // input_side_packets["box_texture_image"] = ;
}

absl::StatusOr<std::map<std::string, mediapipe::Packet>> InitialInputSidePacketsJson(mediapipe::GlCalculatorHelper &gpu_helper) {
    std::map<std::string, mediapipe::Packet> input_side_packets;
    if (!absl::GetFlag(FLAGS_input_side_packets).empty()) {
        std::ifstream f(absl::GetFlag(FLAGS_input_side_packets));
        json data = json::parse(f);
        LOG(INFO) << data.dump();

        input_side_packets["obj_asset_name"] = mediapipe::MakePacket<std::string>(data["obj_asset_name"]);
        input_side_packets["box_asset_name"] = mediapipe::MakePacket<std::string>(data["box_asset_name"]);
        ASSIGN_OR_RETURN(input_side_packets["obj_texture"], CreateRGBPacket(data["obj_texture_image"], gpu_helper));
        ASSIGN_OR_RETURN(input_side_packets["box_texture"], CreateRGBPacket(data["box_texture_image"], gpu_helper));
        input_side_packets["allowed_labels"] = mediapipe::MakePacket<std::string>(data["allowed_labels"]);
        input_side_packets["max_num_objects"] = mediapipe::MakePacket<int>(data["max_num_objects"].get<int>());
        input_side_packets["model_scale"] = mediapipe::MakePacket<float[3]>(0.1, 0.05, 0.1);
        input_side_packets["model_transformation"] = mediapipe::MakePacket<float[16]>(1.0, 0.0, 0.0, 0.0, 0.0,   1.0, 0.0, -10.0, 0.0,   0.0, -1.0, 0.0, 0.0,   0.0, 0.0, 1.0);
    }
    
    return input_side_packets;
}

absl::StatusOr<mediapipe::CalculatorGraphConfig> GetGraphConfig() {
    std::string calculator_graph_config_contents;
    MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
        absl::GetFlag(FLAGS_calculator_graph_config_file),
        &calculator_graph_config_contents));

    LOG(INFO) << "Get calculator graph config contents: \n"
              << calculator_graph_config_contents;

    mediapipe::CalculatorGraphConfig config = 
        mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
            calculator_graph_config_contents);

    return config;
}

absl::StatusOr<mediapipe::GlCalculatorHelper> GetGpuHelper(mediapipe::CalculatorGraph &graph) {
    LOG(INFO) << "Initialize the GPU.";
    ASSIGN_OR_RETURN(auto gpu_resources, mediapipe::GpuResources::Create());
    MP_RETURN_IF_ERROR(graph.SetGpuResources(std::move(gpu_resources)));

    mediapipe::GlCalculatorHelper gpu_helper;
    gpu_helper.InitializeForTest(graph.GetGpuResources().get());

    return gpu_helper;
}

absl::StatusOr<cv::VideoCapture> InitialCamera(bool load_video, bool save_video) {
    cv::VideoCapture capture;
    if (load_video) {
        LOG(INFO) << "Load the video.";
        capture.open(absl::GetFlag(FLAGS_input_video_path));
    } else {
        capture.open(0);
    }
    RET_CHECK(capture.isOpened());
    
    if (!save_video) {
        cv::namedWindow(kWindowName, /*flags=WINDOW_AUTOSIZE*/ 1);
#if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
        capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        capture.set(cv::CAP_PROP_FPS, 30);
#endif
    }
    return capture;
}

absl::StatusOr<cv::Mat> GetFrame(cv::VideoCapture& capture, bool load_video) {
    // Capture opencv camera or video frame.
    // LOG(INFO) << "Get frame";
    cv::Mat camera_frame_raw;
    capture >> camera_frame_raw;
    if (camera_frame_raw.empty()) {
        if (!load_video) {
            LOG(INFO) << "Ignore empty frames from camera.";
            return absl::NotFoundError("Empty frame");
        }
        LOG(INFO) << "Empty frame, end of video reached.";
        return absl::UnavailableError("End of video");
    }

    cv::Mat camera_frame;
    cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGBA);
    if (!load_video) {
        cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);
    }
    return camera_frame;
}

absl::StatusOr<std::unique_ptr<mediapipe::ImageFrame>> GpuBufferToImageFrame(mediapipe::Packet& packet, 
                                                                             mediapipe::GlCalculatorHelper &gpu_helper) {
    std::unique_ptr<mediapipe::ImageFrame> output_frame;

    // Convert GpuBuffer to ImageFrame.
    MP_RETURN_IF_ERROR(gpu_helper.RunInGlContext(
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
        })
    );

    return output_frame;
}

absl::StatusOr<cv::Mat> ConvertImageFrameToMat(std::unique_ptr<mediapipe::ImageFrame>& output_frame) {
    cv::Mat output_frame_mat = mediapipe::formats::MatView(output_frame.get());
    if (output_frame_mat.channels() == 4) {
        cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGBA2BGR);
    } else {
        cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);
    }
    return output_frame_mat;
}

absl::Status RunMPPGraph() {
    ASSIGN_OR_RETURN(auto config, GetGraphConfig());

    LOG(INFO) << "Initialize the calculator graph.";
    mediapipe::CalculatorGraph graph;
    // MP_RETURN_IF_ERROR(graph.Initialize(config));

    LOG(INFO) << "Initialize the GPU.";
    ASSIGN_OR_RETURN(auto gpu_helper, GetGpuHelper(graph));

    ASSIGN_OR_RETURN(auto input_side_packets, InitialInputSidePacketsJson(gpu_helper));

    // LOG(INFO) << "Add image packet (GpuBuffer) into graph.";
    // cv::Mat cv_mat = GetRgb("mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/classic_colors.png");
    // auto box_frame = WrapMatToImageFrame(cv_mat);
    // ASSIGN_OR_RETURN(auto box_gpu_frame, ImageFrameToGpuBuffer(box_frame, gpu_helper));
    // input_side_packets["box_texture_image"] = mediapipe::Adopt(box_gpu_frame.release());
    
    // cv::Mat cv_mat1 = GetRgb("mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/chair/texture.jpg");
    // auto box_frame1 = WrapMatToImageFrame(cv_mat1);
    // ASSIGN_OR_RETURN(auto box_gpu_frame1, ImageFrameToGpuBuffer(box_frame1, gpu_helper));
    // input_side_packets["obj_texture_image"] = mediapipe::Adopt(box_gpu_frame1.release());
    
    return absl::OkStatus();

    LOG(INFO) << "Initialize the camera or load the video.";
    const bool load_video = !absl::GetFlag(FLAGS_input_video_path).empty();
    const bool save_video = !absl::GetFlag(FLAGS_output_video_path).empty();
    ASSIGN_OR_RETURN(auto capture, InitialCamera(load_video, save_video));
    cv::VideoWriter writer;

    LOG(INFO) << "Output stream poller";
    ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                     graph.AddOutputStreamPoller(kOutputStream));
    MP_RETURN_IF_ERROR(graph.StartRun(input_side_packets));

    LOG(INFO) << "Start grabbing and processing frames.";
    bool grab_frames = true;
    while (grab_frames) {
        LOG(INFO) << "Capture frame.";
        // Prepare and add graph input packet.
        size_t frame_timestamp_us =
            (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;

        // Get camera frame
        ASSIGN_OR_RETURN(auto camera_frame, GetFrame(capture, load_video));
        // Convert to ImageFrame
        auto input_frame = WrapMatToImageFrame(camera_frame);
        // Convert to GpuBuffer
        ASSIGN_OR_RETURN(auto gpu_frame, ImageFrameToGpuBuffer(input_frame, gpu_helper));
        MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
                                kInputStream, mediapipe::Adopt(gpu_frame.release())
                                    .At(mediapipe::Timestamp(frame_timestamp_us))));
        
        MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
                                kInputWidthStream, mediapipe::Adopt(new int(640))
                                    .At(mediapipe::Timestamp(frame_timestamp_us))));
          
        MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
                                kInputHeightStream, mediapipe::Adopt(new int(480))
                                    .At(mediapipe::Timestamp(frame_timestamp_us))));

        // Check poller packet
        mediapipe::Packet packet;
        if (!poller.Next(&packet)) break;

        // Get output GpuBuffer frame
        ASSIGN_OR_RETURN(auto output_frame, GpuBufferToImageFrame(packet, gpu_helper));
        // Convert to ImageFrame
        ASSIGN_OR_RETURN(auto output_frame_mat, ConvertImageFrameToMat(output_frame));
        // Save video or display
        // SaveVideoOrDisplay()
        if (save_video) {
            if (!writer.isOpened()) {
                LOG(INFO) << "Prepare video writer.";
                writer.open(absl::GetFlag(FLAGS_output_video_path),
                    mediapipe::fourcc('a', 'v', 'c', '1'),  // .mp4
                    capture.get(cv::CAP_PROP_FPS), output_frame_mat.size());
                RET_CHECK(writer.isOpened());
            }
            writer.write(output_frame_mat);

        } else {
            // LOG(INFO) << "Display output";
            cv::imshow(kWindowName, output_frame_mat);
            // Press any key to exit.
            const int pressed_key = cv::waitKey(5);
            if (pressed_key >= 0 && pressed_key != 255) grab_frames = false;
        }

        // Check OpenCV key press
    }
    LOG(INFO) << "Shutting down.";
    
    // return absl::OkStatus();
    // return OutputSidePackets(graph);
    MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
    return graph.WaitUntilDone();
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::SetCommandLineOption("GLOG_minloglevel", "3");
    absl::ParseCommandLine(argc, argv);
    absl::Status run_status = RunMPPGraph();

    if (!run_status.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << run_status.message();
        return EXIT_FAILURE;
    } else {
        LOG(INFO) << "Success!";
    }
    return EXIT_SUCCESS;
}
