load("//mediapipe/framework/port:build_config.bzl", "mediapipe_proto_library")
load("//mediapipe/framework/tool:mediapipe_graph.bzl", "mediapipe_simple_subgraph")
load(
    "//mediapipe/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

mediapipe_proto_library(
    name = "triangle_renderer_calculator_proto",
    srcs = ["triangle_renderer_calculator.proto"],
    visibility = ["//visibility:public"],
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

cc_library(
    name = "desktop_simple_egl_gpu_calculators",
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
    name = "desktop_simple_egl_gpu_binary_graph",
    graph = "graph_gpu.pbtxt",
    output_name = "desktop_simple_egl_gpu.binarypb",
    deps = [":desktop_simple_egl_gpu_calculators"],
)