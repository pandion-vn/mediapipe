#!/bin/bash
# bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 \
#     --sandbox_debug mymediapipe/examples/desktop/simple_opengl:simple_opengl_demo
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
     --sandbox_debug --verbose_failures mymediapipe/examples/desktop/simple_opengl2:simple_opengl2_demo
