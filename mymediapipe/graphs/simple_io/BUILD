# Copyright 2019 The MediaPipe Authors.
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

licenses(["notice"])  # Apache 2.0

load(
    "//mediapipe/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "desktop_simple_io_cpu_calculators",
    deps = [
        "//mediapipe/calculators/image:image_transformation_calculator",
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/util:annotation_overlay_calculator",
        # "//mediapipe/calculators/video:opencv_video_decoder_calculator",
        # "//mediapipe/calculators/video:opencv_video_encoder_calculator",
    ],
)

cc_library(
    name = "desktop_simple_io_gpu_calculators",
    deps = [
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/image:recolor_calculator",
        "//mediapipe/calculators/util:annotation_overlay_calculator",
        "//mediapipe/calculators/image:image_transformation_calculator",
        # "//mediapipe/gpu:gpu_buffer_to_image_frame_calculator",
        # "//mediapipe/gpu:image_frame_to_gpu_buffer_calculator",
    ],
)

mediapipe_binary_graph(
    name = "desktop_simple_io_cpu_binary_graph",
    graph = "graph_cpu.pbtxt",
    output_name = "desktop_simple_io_cpu.binarypb",
    deps = [":desktop_simple_io_cpu_calculators"],
)

mediapipe_binary_graph(
    name = "desktop_simple_io_gpu_binary_graph",
    graph = "graph_gpu.pbtxt",
    output_name = "desktop_simple_io_gpu.binarypb",
    deps = [":desktop_simple_io_gpu_calculators"],
)