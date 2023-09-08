#include "utils.h"

cv::Mat GetRgbImage(const std::string& path) {
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

mediapipe::Packet CreateFloatArrayPacket(const std::vector<float>& data) {
    float* floats = new float[data.size()];
    std::copy(data.begin(), data.end(), floats);
    return mediapipe::Adopt(reinterpret_cast<float(*)[]>(floats));
}

absl::StatusOr<mediapipe::Packet> CreateRGBPacket(const std::string& path, 
                                                  mediapipe::GlCalculatorHelper &gpu_helper) {
    cv::Mat cv_mat = GetRgbImage(path);
    auto box_frame = WrapMatToImageFrame(cv_mat);
    ASSIGN_OR_RETURN(auto gpu_frame, ImageFrameToGpuBuffer(box_frame, gpu_helper));
    return mediapipe::Adopt(gpu_frame.release());
    // input_side_packets["box_texture_image"] = ;
}