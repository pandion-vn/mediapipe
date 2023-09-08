#ifndef UTILS_H_
#define UTILS_H_

#include <cmath>
#include <cstdlib>
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/gpu/gl_calculator_helper.h"

double degreesToRadians(double degrees);
cv::Mat GetRgbImage(const std::string& path);
mediapipe::ImageFormat::Format GetImageFormat(int image_channels);
std::unique_ptr<mediapipe::ImageFrame> WrapMatToImageFrame(cv::Mat& camera_frame);
absl::StatusOr<std::unique_ptr<mediapipe::GpuBuffer>> 
    ImageFrameToGpuBuffer(std::unique_ptr<mediapipe::ImageFrame> &input_frame, 
                          mediapipe::GlCalculatorHelper &gpu_helper);

mediapipe::Packet CreateImageFramePacket(cv::Mat& input);
mediapipe::Packet CreateFloatArrayPacket(const std::vector<float>& data);
absl::StatusOr<mediapipe::Packet> CreateRGBPacket(const std::string& path, 
                                                  mediapipe::GlCalculatorHelper &gpu_helper);

#endif  // UTILS_H_