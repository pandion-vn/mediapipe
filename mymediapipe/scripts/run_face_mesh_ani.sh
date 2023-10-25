# GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_shader/face_mesh_demo \
#         --calculator_graph_config_file=mymediapipe/graphs/pose_ani/face_mesh_detection_gpu.pbtxt \
#         --input_video_path=mymediapipe/assets/video/demo.mp4 \
#         --output_video_path=mymediapipe/examples/desktop/demo_output.mp4
# GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_shader/face_mesh_demo \
#         --calculator_graph_config_file=mymediapipe/graphs/pose_ani/face_mesh_gpu.pbtxt
GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_shader/face_mesh_demo \
        --calculator_graph_config_file=mymediapipe/graphs/pose_ani/face_mesh_detection_gpu.pbtxt
