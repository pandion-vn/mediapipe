load("//mediapipe/framework/port:build_config.bzl", "mediapipe_proto_library")
load("//mediapipe/framework:mediapipe_register_type.bzl", "mediapipe_register_type")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

### General calculator superclasses
cc_library(
    name = "gl_base_lib",
    srcs = [
        "lib/stb_image.cc",
        "lib/shader.h",
        "lib/camera.h",
        "lib/mesh.h",
        "lib/model.h",
        "lib/video_scene.h",
        "lib/model_scene.h",
        "lib/model_scene_a.h",
        "lib/framebuffer_target.h",
        "lib/bone.h",
        "lib/animation.h",
        "lib/animator.h",
        "lib/util.cc",
        "lib/my_pose.h",
        "lib/my_math.h",
        # "lib/tiny_obj_loader.cc",
    ],
    hdrs = [
        "lib/stb_image.h",
        "lib/shader.h",
        "lib/camera.h",
        "lib/mesh.h",
        "lib/model.h",
        "lib/video_scene.h",
        "lib/model_scene.h",
        "lib/model_scene_a.h",
        "lib/framebuffer_target.h",
        "lib/bone.h",
        "lib/animation.h",
        "lib/animator.h",
        "lib/my_pose.h",
        "lib/my_math.h",
        # "lib/rapidobj.hpp",
        # "lib/tiny_obj_loader.h",
        "lib/util.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//third_party:assimp",
        "//third_party:opengl_glm",
        "//mediapipe/gpu:shader_util",
    ],
)

cc_library(
    name = "gl_base_calculator",
    srcs = ["gl_base_calculator.cc"],
    hdrs = ["gl_base_calculator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/port:status",
        ":gl_base_lib",
    ],
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
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_11_coordinate_system_calculator",
    srcs = ["gl_11_coordinate_system_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_11a_coordinate_system_calculator",
    srcs = ["gl_11a_coordinate_system_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//mediapipe/framework/formats:matrix_data_cc_proto",
        "//mediapipe/modules/face_geometry/protos:environment_cc_proto",
        "//mediapipe/modules/face_geometry/protos:face_geometry_cc_proto",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_11b_coordinate_system_calculator",
    srcs = ["gl_11b_coordinate_system_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//mediapipe/framework/formats:matrix_data_cc_proto",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/modules/face_geometry/protos:environment_cc_proto",
        "//mediapipe/modules/face_geometry/protos:face_geometry_cc_proto",
        "//mediapipe/modules/face_geometry/libs:procrustes_solver",
        "//third_party:opengl_glm",
        "@com_google_absl//absl/memory",
        "@eigen_archive//:eigen3",
        ":gl_base_calculator",
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_12_camera_view_calculator",
    srcs = ["gl_12_camera_view_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_14_lighting_material_calculator",
    srcs = ["gl_14_lighting_material_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_15_lighting_maps_calculator",
    srcs = ["gl_15_lighting_maps_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        ":gl_base_calculator",
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_16_model_loading_calculator",
    srcs = ["gl_16_model_loading_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        "//third_party:assimp",
        ":gl_base_calculator",
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_17_skeletal_animation_calculator",
    srcs = ["gl_17_skeletal_animation_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        "//third_party:assimp",
        ":gl_base_calculator",
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_18_camera_skeletal_animation_calculator",
    srcs = ["gl_18_camera_skeletal_animation_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_simple_shaders",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        "//third_party:assimp",
        ":gl_base_calculator",
        ":gl_base_lib",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gl_18a_camera_skeletal_animation_calculator",
    srcs = ["gl_18a_camera_skeletal_animation_calculator.cc"],
    deps = [
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
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
        ":gl_base_calculator",
        ":gl_base_lib",
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
