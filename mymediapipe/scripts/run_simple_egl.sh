GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_egl/simple_egl_demo \
        --calculator_graph_config_file=mymediapipe/graphs/simple_egl/graph_gpu.pbtxt \
        --input_video_path=mymediapipe/assets/video/demo.mp4 \
        --output_video_path=mymediapipe/examples/desktop/simple_egl/demo_output.mp4
