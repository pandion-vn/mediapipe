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

#ifdef __APPLE__

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/gl3ext.h>
#define GLFW_INCLUDE_GLCOREARB
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE

#else // Not APPLE

// #include <GL/glew.h> # disable by mediapipe
// #include <GL/glext.h>
// #include <GL/glew.h>
// #include <GLES3/gl32.h>
// #include <GL/gl.h>
// #include <GL/glew.h>
// #include <GLES3/gl3.h>
// #define GLFW_INCLUDE_ES2

#endif

// #define GLFW_INCLUDE_NONE
// #define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

// #include <Effekseer.h>
// #include <Effekseer.Modules.h>
// #include <Effekseer.SIMD.h>
// #include <EffekseerRendererGL.h>
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
// #include "mediapipe/framework/formats/image_frame.h"
// #include "mediapipe/framework/formats/image_frame_opencv.h"
// #include "mediapipe/framework/port/opencv_highgui_inc.h"
// #include "mediapipe/framework/port/opencv_imgproc_inc.h"
// #include "mediapipe/framework/port/opencv_video_inc.h"
// #include "mediapipe/framework/port/parse_text_proto.h"
// #include "mediapipe/framework/port/status.h"
// #include "mediapipe/framework/formats/detection.pb.h"
// #include "mediapipe/framework/formats/location.h"
// #include "mediapipe/framework/formats/landmark.pb.h"
// #include "mediapipe/gpu/gl_calculator_helper.h"
// #include "mediapipe/gpu/gpu_buffer.h"
// #include "mediapipe/gpu/gpu_shared_data_internal.h"

// #include "constants.h"
// #include "tempo.h"

#endif
