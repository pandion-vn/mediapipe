#!/bin/bash
BAZEL_NO_APPLE_CPP_TOOLCHAIN=1 \
    bazel build --sandbox_debug \
    mymediapipe/examples/desktop/simple_opengl:simple_opengl_demo
