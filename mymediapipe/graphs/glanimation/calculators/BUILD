load("//mediapipe/framework/port:build_config.bzl", "mediapipe_proto_library")
load("//mediapipe/framework:mediapipe_register_type.bzl", "mediapipe_register_type")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

### General calculator superclasses

cc_library(
    name = "gl_base_calculator",
    srcs = ["gl_base_calculator.cc"],
    hdrs = ["gl_base_calculator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/port:status",
    ],
)

cc_library(
    name = "stb_image",
    srcs = ["lib/stb_image.h"],
    hdrs = ["lib/stb_image.h"],
    visibility = ["//visibility:public"],
    deps = [],
)

cc_library(
    name = "opengl_shader_calculator",
    srcs = ["opengl_shader_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_02_dot_calculator",
    srcs = ["gl_02_dot_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_03_triangle_calculator",
    srcs = ["gl_03_triangle_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_05_triangle_uniform_calculator",
    srcs = ["gl_05_triangle_uniform_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_06_triangle_transformation_calculator",
    srcs = ["gl_06_triangle_transformation_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_09_triangle_interpolation_calculator",
    srcs = ["gl_09_triangle_interpolation_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_10_triangle_texture_calculator",
    srcs = ["gl_10_triangle_texture_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
        ":stb_image",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_shader_calculator",
    srcs = ["gl_shader_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        ":gl_base_calculator",
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
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        ":gl_base_calculator",
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
