# SimpleIO

## Build

$ bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
  mymediapipe/examples/desktop/simple_io:simple_io

$ bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS \
  --copt -DEGL_NO_X11 mymediapipe/examples/desktop/simple_io:simple_io_demo_gpu

## Run

$ export GLOG_logtostderr=1
$ bazel-bin/mymediapipe/examples/desktop/simple_io/simple_io \
  --calculator_graph_config_file=mymediapipe/graphs/simple_io/simple_io_graph.pbtxt \
  --input_side_packets=input_video_path=mediapipe/examples/desktop/object_detection/test_video.mp4\
  ,output_video_path=mymediapipe/examples/desktop/simple_io/output_video.mp4

$ bazel-bin/myMediapipe/examples/desktop/simple_io/simple_io_demo_gpu \
  --calculator_graph_config_file=myMediapipe/graphs/simple_io/simple_io_graph.pbtxt \
  --input_video_path=mediapipe/examples/desktop/object_detection/test_video.mp4 \
  --output_video_path=videos/simple_io_output_video.mp4