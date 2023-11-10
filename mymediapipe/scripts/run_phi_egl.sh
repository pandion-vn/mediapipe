GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/phi_egl/phi_egl_demo \
        --calculator_graph_config_file=mymediapipe/graphs/phi_egl/phi_egl_overlay.pbtxt \
        --input_video_path=mymediapipe/assets/video/vf-pose.mp4
        # --output_video_path=mymediapipe/examples/desktop/demo_output.mp4
# GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_shader/pose_ani_demo \
#         --calculator_graph_config_file=mymediapipe/graphs/pose_ani/pose_ani_gpu.pbtxt
