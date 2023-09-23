#!/bin/bash

# bazel build -c opt \
#     --host_crosstool_top=@bazel_tools//tools/cpp:toolchain \
#     --compiler=gcc \
#     --cpu=${BAZEL_CPU} \
#     --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 --sandbox_debug \
#     mymediapipe/examples/desktop/simple_shader:opengl_shader_demo

bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 --sandbox_debug \
    mymediapipe/examples/desktop/simple_shader:opengl_shader_demo