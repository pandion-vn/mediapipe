# Copyright 2020 The MediaPipe Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load(
    "//mediapipe/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "pose_tracking_gpu_deps",
    deps = [
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/graphs/pose_tracking/subgraphs:pose_renderer_gpu",
        "//mediapipe/modules/pose_landmark:pose_landmark_gpu",
        "//mymediapipe/calculators/debugger:common_debugger_calculator",
        "//mediapipe/modules/face_geometry:env_generator_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_11b_coordinate_system_calculator",
        # "//mymediapipe/graphs/glanimation/calculators:gl_18_camera_skeletal_animation_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_18a_camera_skeletal_animation_calculator",
    ],
)

cc_library(
    name = "face_mesh_gpu_deps",
    deps = [
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/core:immediate_mux_calculator",
        "//mediapipe/calculators/image:image_properties_calculator",
        "//mediapipe/graphs/face_effect/subgraphs:single_face_geometry_from_detection_gpu",
        "//mediapipe/graphs/face_effect/subgraphs:single_face_geometry_from_landmarks_gpu",
        "//mediapipe/modules/face_geometry:effect_renderer_calculator",
        "//mediapipe/modules/face_geometry:env_generator_calculator",
        "//mymediapipe/calculators/debugger:common_debugger_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_11a_coordinate_system_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_16_model_loading_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_18_camera_skeletal_animation_calculator",
    ],
)


# cc_library(
#     name = "pose_tracking_cpu_deps",
#     deps = [
#         "//mediapipe/calculators/core:constant_side_packet_calculator",
#         "//mediapipe/calculators/core:flow_limiter_calculator",
#         "//mediapipe/graphs/pose_tracking/subgraphs:pose_renderer_cpu",
#         "//mediapipe/modules/pose_landmark:pose_landmark_cpu",
#     ],
# )

# mediapipe_binary_graph(
#     name = "pose_tracking_cpu_binary_graph",
#     graph = "pose_tracking_cpu.pbtxt",
#     output_name = "pose_tracking_cpu.binarypb",
#     deps = [":pose_tracking_cpu_deps"],
# )
