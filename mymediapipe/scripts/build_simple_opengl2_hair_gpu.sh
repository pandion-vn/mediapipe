#!/bin/bash

# set -e
# build_only=false
# run_only=false

# while [[ -n $1 ]]; do
#      case $1 in
#           -d)
#                shift
#                out_dir=$1
#                ;;
#           -b)
#                build_only=true
#                ;;
#           -r)
#                run_only=true
#                ;;
#           *)
#                echo "Unsupported input argument $1."
#                exit 1
#                ;;
#      esac
#      shift
# done

# if [[ $run_only == false ]]; then
# fi

bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11  \
     --sandbox_debug mymediapipe/examples/desktop/simple_opengl2:simple_opengl2_hair_seg_demo_gpu
# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
#      --sandbox_debug --verbose_failures mymediapipe/examples/desktop/simple_opengl2:simple_opengl2_hair_seg_demo
# bazel build \
#      --sandbox_debug --verbose_failures mymediapipe/examples/desktop/simple_opengl2:simple_opengl2_demo
