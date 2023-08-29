#ifndef COMMON_H
#define COMMON_H

#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <cmath>

#include <GL/glew.h> /* include GLEW and new version of GL on Windows */
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> /* GLFW helper library */

#include "opencv2/opencv.hpp"

#include "glm/glm.hpp"

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/framework/port/logging.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
// #include "mediapipe/framework/port/opencv_highgui_inc.h"
// #include "mediapipe/framework/port/opencv_imgproc_inc.h"
// #include "mediapipe/framework/port/opencv_video_inc.h"
// #include "mediapipe/framework/port/parse_text_proto.h"
// #include "mediapipe/framework/formats/detection.pb.h"
// #include "mediapipe/framework/formats/location.h"
// #include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"
#include "mediapipe/util/resource_util.h"

#include "constants.h"
// #include "tempo.h"

#endif
