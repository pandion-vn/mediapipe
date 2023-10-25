#!/bin/bash
# bazel build -c opt --macos_sdk_version=10.14 --define MEDIAPIPE_DISABLE_GPU=1 \
#     mymediapipe/examples/desktop/simple_math:simple_math_demo
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
    mymediapipe/examples/desktop/simple_math:simple_math_demo