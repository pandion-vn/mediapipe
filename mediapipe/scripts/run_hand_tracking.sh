# build
# bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADER --copt -DEGL_NO_X11 --sandbox_debug mediapipe/examples/desktop/hand_tracking:hand_tracking_gpu
# run
GLOG_logtostderr=1 bazel-bin/mediapipe/examples/desktop/hand_tracking/hand_tracking_gpu \
--calculator_graph_config_file=mediapipe/graphs/hand_tracking/hand_tracking_desktop_live_gpu.pbtxt