#!/bin/bash
bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 --sandbox_debug \
    mymediapipe/examples/desktop/simple_io:objectron3d_graph_gpu_demo