load("//mediapipe/framework/port:build_config.bzl", "mediapipe_proto_library")
load("//mediapipe/framework/tool:mediapipe_graph.bzl", "mediapipe_simple_subgraph")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

mediapipe_proto_library(
    name = "triangle_renderer_calculator_proto",
    srcs = ["triangle_renderer_calculator.proto"],
    deps = [
        "//mediapipe/framework:calculator_options_proto",
    ],
)

cc_library(
    name = "triangle_renderer_calculator",
    srcs = ["triangle_renderer_calculator.cc"],
    deps = [
        ":triangle_renderer_calculator_cc_proto",
        # "//mediapipe/framework:calculator_framework",
        # "//mediapipe/framework/formats:image_frame",
        # "//mediapipe/framework/formats:image_frame_opencv",
        # "//mediapipe/framework/port:opencv_core",
        # "//mediapipe/framework/port:opencv_imgcodecs",
        # "//mediapipe/framework/port:opencv_imgproc",
        # "//mediapipe/framework/port:ret_check",
        # "//mediapipe/framework/port:status",
        # "//mediapipe/framework/port:statusor",
        # "//mediapipe/gpu:gl_calculator_helper",
        # "//mediapipe/gpu:gpu_buffer",
        # "//mediapipe/modules/face_geometry/libs:effect_renderer",
        # "//mediapipe/modules/face_geometry/libs:validation_utils",
        # "//mediapipe/modules/face_geometry/protos:environment_cc_proto",
        # "//mediapipe/modules/face_geometry/protos:face_geometry_cc_proto",
        # "//mediapipe/modules/face_geometry/protos:mesh_3d_cc_proto",
        # "//mediapipe/util:resource_util",
        "@com_google_absl//absl/types:optional",
    ],
    alwayslink = 1,
)