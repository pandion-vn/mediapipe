# GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_io/objectron3d_graph_gpu_demo \
#         --calculator_graph_config_file=mediapipe/graphs/object_detection_3d/objectron_desktop_gpu.pbtxt \
#         --input_video_path=mymediapipe/assets/video/demo-chair-1.mp4 \
#         --output_video_path=mymediapipe/examples/desktop/simple_io/demo_chair_output.mp4 \
#         --input_side_packets=box_texture_image=mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/classic_colors.png,\
# box_asset_name=mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/box.obj.uuu,\
# obj_texture_image=mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/chair/texture.jpg,\
# obj_asset_name=mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/chair/model.obj.uuu


GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_io/instant_motion_gpu_demo \
        --calculator_graph_config_file=mediapipe/graphs/instant_motion_tracking/instant_motion_tracking.pbtxt \
        --input_video_path=mymediapipe/assets/video/demo-chair-1.mp4 \
        --output_video_path=mymediapipe/examples/desktop/simple_io/demo_chair_output.mp4 \
        --input_side_packets=mymediapipe/scripts/instant_tracking_side_packets.json

