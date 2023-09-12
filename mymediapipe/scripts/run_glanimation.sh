GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_glanimation/glanimation_demo \
        --calculator_graph_config_file=mymediapipe/graphs/glanimation/simple_glanimation_overlay.pbtxt \
        --input_video_path=mymediapipe/assets/video/demo-chair-1.mp4 \
        --output_video_path=mymediapipe/examples/desktop/simple_glanimation/demo_output.mp4 \
        --input_side_packets=mymediapipe/scripts/instant_tracking_side_packets.json
