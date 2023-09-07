GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_io/simple_graph_gpu_demo \
        --calculator_graph_config_file=mymediapipe/graphs/simple_io/graph_gpu.pbtxt \
        --input_video_path=mymediapipe/assets/video/demo.mp4 \
        --output_video_path=mymediapipe/examples/desktop/simple_io/demo_output.mp4
