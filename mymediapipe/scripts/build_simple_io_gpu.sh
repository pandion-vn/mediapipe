#!/bin/bash
bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 \
    mymediapipe/examples/desktop/simple_io:simple_io_demo_gpu