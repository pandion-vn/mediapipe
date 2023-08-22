#!/bin/bash
# bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 \
#     --sandbox_debug mymediapipe/examples/desktop/simple_opengl:simple_opengl_demo
bazel build \
     --sandbox_debug --verbose_failures mymediapipe/examples/desktop/simple_opengl:simple_opengl_demo
