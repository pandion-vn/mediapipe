load("//mediapipe/framework/port:build_config.bzl", "mediapipe_proto_library")
load("//mediapipe/framework:mediapipe_register_type.bzl", "mediapipe_register_type")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "gl_ik_opengl_calculator",
    srcs = ["gl_ik_opengl_calculator.cc"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//mediapipe/framework/formats:matrix_data_cc_proto",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/modules/face_geometry/protos:environment_cc_proto",
        "//mediapipe/modules/face_geometry/protos:face_geometry_cc_proto",
        "//mediapipe/modules/face_geometry/libs:procrustes_solver",
        "//third_party:opengl_glm",
        "//third_party:assimp",
        "@com_google_absl//absl/memory",
        "@eigen_archive//:eigen3",
        "//mymediapipe/calculators/ik:ik_calculators",
        # ":gl_base_calculator",
        # ":gl_base_lib",
    ],
    alwayslink = 1,
)