#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// #include "json.hpp"
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
// #include "mediapipe/graphs/instant_motion_tracking/calculators/sticker_buffer.pb.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"
// #include "utils.h"

constexpr char kWindowName[] = "Simple Graph GPU Demo";
constexpr char kInputStream[] = "input_video";
constexpr char kInputWidthStream[] = "input_width";
constexpr char kInputHeightStream[] = "input_height";
constexpr char kOutputStream[] = "output_video";

ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");

ABSL_FLAG(std::string, input_video_path, "",
          "Full path of video to load. "
          "If not provided, attempt to use a webcam.");
ABSL_FLAG(std::string, output_video_path, "",
          "Full path of where to save result (.mp4 only). "
          "If not provided, show result in a window.");

ABSL_FLAG(std::string, input_side_packets, "",
          "Comma-separated list of key=value pairs specifying side packets "
          "for the CalculatorGraph. All values will be treated as the "
          "string type even if they represent doubles, floats, etc.");

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
    // LOG(INFO) << "Initialize the GPU.";
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

cv::Mat GetRgbImage(const std::string& path) {
    cv::Mat bgr = cv::imread(path);
    cv::Mat rgb;
    cv::cvtColor(bgr, rgb, cv::COLOR_BGR2RGBA);
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

absl::StatusOr<mediapipe::Packet> CreateRGBPacket(const std::string& path, 
                                                  mediapipe::GlCalculatorHelper &gpu_helper) {
    cv::Mat cv_mat = GetRgbImage(path);
    auto box_frame = WrapMatToImageFrame(cv_mat);
    ASSIGN_OR_RETURN(auto gpu_frame, ImageFrameToGpuBuffer(box_frame, gpu_helper));
    return mediapipe::Adopt(gpu_frame.release());
    // input_side_packets["box_texture_image"] = ;
}

absl::Status RunMPPGraph() {
    ASSIGN_OR_RETURN(auto config, GetGraphConfig());

    LOG(INFO) << "Initialize the calculator graph.";
    mediapipe::CalculatorGraph graph;
    MP_RETURN_IF_ERROR(graph.Initialize(config));

    LOG(INFO) << "Initialize the GPU.";
    ASSIGN_OR_RETURN(auto gpu_helper, GetGpuHelper(graph));

    LOG(INFO) << "Initialize the camera or load the video.";
    const bool load_video = !absl::GetFlag(FLAGS_input_video_path).empty();
    const bool save_video = !absl::GetFlag(FLAGS_output_video_path).empty();
    ASSIGN_OR_RETURN(auto capture, InitialCamera(load_video, save_video));
    cv::VideoWriter writer;

    std::map<std::string, mediapipe::Packet> input_side_packets;
    // NG
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/instantmotiontracking/robot/robot_texture.png", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/instantmotiontracking/robot/robot.obj.uuu");
    // NG
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/instantmotiontracking/gif/default_gif_texture.png", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/instantmotiontracking/gif/gif.obj.uuu");
    // OK
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/objectron/classic_colors.png", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/objectron/box.obj.uuu");
    // OK
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/objectron/chair/texture.jpg", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/objectron/chair/model.obj.uuu");
    // NG
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/objectron/cup/texture.jpg", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/objectron/cup/model.obj.uuu");
    // NG
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/objectron/camera/texture.jpg", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/objectron/camera/model.obj.uuu");
    // OK
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/objectron/sneaker/texture.jpg", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/objectron/sneaker/model.obj.uuu");
    // OK
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/objectron/classic_colors.png", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/ironman/cube.obj.uuu");
    // OK
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/instantmotiontracking/robot/robot_texture.png", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/ironman/untitled.obj.uuu");
    // OK
    // ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/instantmotiontracking/robot/robot_texture.png", gpu_helper));
    // input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/cube/cube.obj.uuu");
    // OK
    ASSIGN_OR_RETURN(input_side_packets["texture_3d"], CreateRGBPacket("mymediapipe/assets/instantmotiontracking/robot/robot_texture.png", gpu_helper));
    input_side_packets["asset_3d"] = mediapipe::MakePacket<std::string>("mymediapipe/assets/sword/Sting-Sword.obj.uuu");

    LOG(INFO) << "Output stream poller";
    ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                     graph.AddOutputStreamPoller(kOutputStream));
    MP_RETURN_IF_ERROR(graph.StartRun(input_side_packets));

    LOG(INFO) << "Start grabbing and processing frames.";
    bool grab_frames = true;
    while (grab_frames) {
        // LOG(INFO) << "Capture frame.";
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
        
        // MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
        //                         kInputWidthStream, mediapipe::Adopt(new int(640))
        //                             .At(mediapipe::Timestamp(frame_timestamp_us))));
          
        // MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
        //                         kInputHeightStream, mediapipe::Adopt(new int(480))
        //                             .At(mediapipe::Timestamp(frame_timestamp_us))));

        // sticker_sentinel
        // MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
        //                         "sticker_sentinel", mediapipe::Adopt(new int(0)).At(mediapipe::Timestamp(frame_timestamp_us))));
        // sticker_proto_string
        // mediapipe::StickerRoll stickerRoll;
        // mediapipe::Sticker* sticker = stickerRoll.add_sticker();
        // mediapipe::Sticker sticker;
        // sticker->set_id(0);
        // sticker->set_x(0);
        // sticker->set_y(0);
        // sticker->set_rotation(0);
        // sticker->set_scale(1);
        // sticker->set_render_id(0);
        // stickerRoll.add_sticker(sticker);
        // std::string data;
        // stickerRoll.SerializeToString(&data);
        // MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
        //                         "sticker_proto_string", mediapipe::MakePacket<std::string>(data)
        //                                                 .At(mediapipe::Timestamp(frame_timestamp_us))));

        // imu_rotation_matrix
        // MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
        //                         "imu_rotation_matrix", imu_rotation_matrix_packet.At(mediapipe::Timestamp(frame_timestamp_us))));
        // gif_texture
        // MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
        //                         "gif_texture", default_gif_texture_packet.At(mediapipe::Timestamp(frame_timestamp_us))));
        // gif_aspect_ratio
        // MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
        //                         "gif_aspect_ratio", mediapipe::Adopt(new float(341.0 / 321.0)).At(mediapipe::Timestamp(frame_timestamp_us))));
        // LOG(INFO) << "AddPacketToInputStream";

        // Check poller packet
        mediapipe::Packet packet;
        if (!poller.Next(&packet)) break;
        // LOG(INFO) << "GpuBufferToImageFrame";

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
