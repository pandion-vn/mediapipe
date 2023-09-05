# build
# bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADER --copt -DEGL_NO_X11 --sandbox-debug mediapipe/examples/desktop/sobel_edge:sobel_edge_gpu
# run
GLOG_logtostderr=1 bazel-bin/mediapipe/examples/desktop/sobel_edge/sobel_edge_gpu --calculator_graph_config_file=mediapipe/graphs/edge_detection/edge_detection_mobile_gpu.pbtxt