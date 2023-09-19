load("//mediapipe/framework/port:build_config.bzl", "mediapipe_proto_library")
load("//mediapipe/framework:mediapipe_register_type.bzl", "mediapipe_register_type")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "gl_shader_calculator",
    srcs = ["gl_shader_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_calculator",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
    ],
    alwayslink = 1,
)

cc_library(
    name = "lens_flare_shader_calculator",
    srcs = ["lens_flare_shader_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_calculator",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
    ],
    alwayslink = 1,
)

cc_library(
    name = "triangle_shader_calculator",
    srcs = ["triangle_shader_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_calculator",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
    ],
    alwayslink = 1,
)
