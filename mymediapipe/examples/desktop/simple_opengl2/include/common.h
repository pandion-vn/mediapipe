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

#include <TargetConditionals.h>

#if TARGET_OS_OSX

#define HAS_NSGL 1

#include <OpenGL/OpenGL.h>

#if CGL_VERSION_1_3
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif  // CGL_VERSION_1_3

#else

#define HAS_EAGL 1

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#endif  // TARGET_OS_OSX

#define GL_SILENCE_DEPRECATION
// #define GLFW_INCLUDE_GLCOREARB
// #define glGenVertexArrays glGenVertexArraysAPPLE
// #define glBindVertexArray glBindVertexArrayAPPLE
// #define glDeleteVertexArrays glDeleteVertexArraysAPPLE

#else // Not APPLE
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
// #include <GL/glut.h>

#endif

// #define GLFW_INCLUDE_NONE
// #define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

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

#include "constants.h"
// #include "tempo.h"

#endif
