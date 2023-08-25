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

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "simple_opengl2",
    srcs = [
        "lib/stb_image.cpp",
        "main.cc",
        "src/sandbox.cc",
        "src/texture.cc",
        "src/sprite_renderer.cc",
        "src/neon_renderer.cc",
        "src/resource_manager.cc",
        "src/shader.cc",
    ],
    hdrs = [
        "lib/stb_image.h",
        "include/common.h",
        "include/constants.h",
        "include/sandbox.h",
        "include/texture.h",
        "include/sprite_renderer.h",
        "include/neon_renderer.h",
        "include/resource_manager.h",
        "include/shader.h",
    ],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework:calculator_graph",
        "//mediapipe/framework/port:logging",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        "//mediapipe/framework/port:commandlineflags",
        # "//mediapipe/calculators/core:pass_through_calculator",
        # "//mediapipe/framework/formats:image_frame",
        # "//mediapipe/framework/formats:image_frame_opencv",
        # "//mediapipe/framework/port:logging",
        # "//mediapipe/framework/port:parse_text_proto",
        # "//mediapipe/framework/port:status",
        # "//mediapipe/framework/port:opencv_highgui",
        # "//mediapipe/framework/port:opencv_imgproc",
        # "//mediapipe/framework/port:opencv_video",
        # "//mediapipe/gpu:gl_base",
        # "//mediapipe/gpu:gl_calculator_helper",
        # "//mediapipe/gpu:gpu_buffer",
        # "//mediapipe/gpu:gpu_shared_data_internal",
        "//third_party:opencv",
        "//third_party:opengl",
        "//third_party:opengl_glfw",
        "//third_party:opengl_glm",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
    ],
    linkopts = [
        # "-lstdc++fs",
    ],
    data = [
        # "assets/shaders/sprite.fs",
        # "assets/shaders/sprite.vs",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "simple_opengl2_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_opengl2:simple_opengl2",
        "//mymediapipe/graphs/simple_io:desktop_simple_io_cpu_calculators",
        # "//mymediapipe/graphs/simple_io:desktop_simple_io_gpu_calculators",
    ],
)