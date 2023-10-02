GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_shader/opengl_shader_demo \
        --calculator_graph_config_file=mymediapipe/graphs/glanimation/opengl_shader_overlay.pbtxt \
        --input_video_path=mymediapipe/assets/video/demo.mp4 \
        --output_video_path=mymediapipe/examples/desktop/demo_output.mp4
