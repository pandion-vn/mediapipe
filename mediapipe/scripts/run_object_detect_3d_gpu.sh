# build
# bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADER --copt -DEGL_NO_X11 --sandbox_debug mediapipe/examples/desktop/object_detection_3d:objectron_gpu
# run
GLOG_logtostderr=1 bazel-bin/mediapipe/examples/desktop/object_detection_3d/objectron_gpu \
--calculator_graph_config_file=mediapipe/graphs/object_detection_3d/objectron_desktop_gpu.pbtxt \
--input_side_packets=box_texture_path=mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/classic_colors.png,\
box_asset_name=mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/box.obj.uuu,\
obj_texture_path=mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/chair/texture.jpg,\
obj_asset_name=mediapipe/examples/android/src/java/com/google/mediapipe/apps/objectdetection3d/assets/chair/model.obj.uuu