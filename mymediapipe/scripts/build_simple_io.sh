#!/bin/bash
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
    mymediapipe/examples/desktop/simple_io:simple_io