# Copyright 2023 nghiach.
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

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "simple_graph",
    srcs = ["main_simple_graph.cc"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:map_util",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/framework/port:statusor",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "simple_graph_gpu",
    hdrs = [
        "json.hpp",
        "utils.h",
    ],
    srcs = [
        "main_simple_graph_gpu.cc",
        "utils.cc",
    ],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:map_util",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/framework/port:statusor",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/framework/port:opencv_video",
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/util:resource_util",
        "//third_party:opencv",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "instant_tracking",
    hdrs = [
        "json.hpp",
        "utils.h",
    ],
    srcs = [
        "main_instant_tracking.cc",
        "utils.cc",
    ],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:map_util",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/framework/port:statusor",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/framework/port:opencv_video",
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/util:resource_util",
        "//third_party:opencv",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "simple_io",
    srcs = ["main.cc"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:map_util",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/framework/port:statusor",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
        # "//third_party:opencv",
        # "//third_party:opengl",
        # "//third_party:opengl_glfw",
        # "@system_headers//:headers",
        # "@system_libs//:opengl",
        # "@effekseer_libs//:glfw-dev",
        # "@effekseer_libs//:effekseer-dev",
    ],
    data = [
        # "//myMediapipe/examples/desktop/assets:Laser01.efk",
        # "//myMediapipe/examples/desktop/assets:Laser03.efk",
        # "//myMediapipe/examples/desktop/assets:SimpleFragmentShader.fragmentshader",
        # "//myMediapipe/examples/desktop/assets:SimpleVertexShader.vertexshader",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "simple_io_demo",
    deps = [
        "//mediapipe/examples/desktop:simple_run_graph_main",
        "//myMediapipe/graphs/simple_io:desktop_simple_io",
    ],
)

cc_binary(
    name = "simple_graph_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_io:simple_graph",
        "//mymediapipe/graphs/simple_io:hello_world_calculators",
    ],
)

cc_binary(
    name = "simple_graph_gpu_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_io:simple_graph_gpu",
        "//mymediapipe/graphs/simple_io:desktop_simple_io_gpu_calculators",
    ],
)

cc_binary(
    name = "objectron3d_graph_gpu_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_io:simple_graph_gpu",
        "//mediapipe/graphs/object_detection_3d:desktop_gpu_calculators",
    ],
)

cc_binary(
    name = "instant_motion_gpu_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_io:instant_tracking",
        "//mediapipe/graphs/instant_motion_tracking:instant_motion_tracking_deps",
    ],
)