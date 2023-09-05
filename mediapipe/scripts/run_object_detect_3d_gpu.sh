# build
# bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADER --copt -DEGL_NO_X11 --sandbox_debug mediapipe/examples/desktop/object_detection_3d:objectron_gpu
# run
GLOG_logtostderr=1 bazel-bin/mediapipe/examples/desktop/object_detection_3d/objectron_gpu --calculator_graph_config_file=mediapipe/graphs/object_detection_3d/object_occlusion_tracking.pbtxt