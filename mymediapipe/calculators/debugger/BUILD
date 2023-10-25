licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:private"])

exports_files(["LICENSE"])

load("//mediapipe/framework/port:build_config.bzl", "mediapipe_cc_proto_library")

cc_library(
    name = "common_debugger_calculator",
    srcs = ["common_debugger_calculator.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework:calculator_options_cc_proto",
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/formats:detection_cc_proto",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/modules/face_geometry/protos:environment_cc_proto",
        "//mediapipe/modules/face_geometry/protos:face_geometry_cc_proto",
        "@com_google_absl//absl/memory",
    ],
    alwayslink = 1,
)