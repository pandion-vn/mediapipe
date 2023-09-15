GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_shader/shader_lens_flare_demo \
        --calculator_graph_config_file=mymediapipe/graphs/glanimation/simple_lens_flare_shader_overlay.pbtxt \
        --input_video_path=mymediapipe/assets/video/demo-chair-1.mp4 \
        --output_video_path=mymediapipe/examples/desktop/demo_output.mp4
