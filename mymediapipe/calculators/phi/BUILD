licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:private"])

exports_files(["LICENSE"])

load("//mediapipe/framework/port:build_config.bzl", "mediapipe_cc_proto_library")

cc_library(
    name = "phi_calculators",
    visibility = ["//visibility:public"],
    srcs = [
        "helper.cc",
        "model.cc",
        "texture.cc",
        "stb_image.cc",
    ],
    hdrs = [
        "common.h",
        "camera.h",
        "helper.h",
        "bone.h",
        "mesh.h",
        "model.h",
        "skeleton.h",
        "texture.h",
        "rendering/material.h",
        "rendering/light.h",
        "simulation.h",
        "phi_shader.h",
        "stb_image.h",
        "vertex.h",
        "controller/abstract_controller.h",
        "controller/fem_controller.h",
        "controller/ani_controller.h",
        "animation/i_animation.h",
        "animation/ik_animator.h",
    ],
    deps = [
        # "//mediapipe/framework:calculator_framework",
        # "//mediapipe/framework/formats:image_frame",
        # "//mediapipe/framework/formats:image_frame_opencv",
        # "//mediapipe/framework/port:file_helpers",
        # "//mediapipe/framework/port:map_util",
        # "//mediapipe/framework/port:parse_text_proto",
        # "//mediapipe/framework/port:ret_check",
        # "//mediapipe/framework/port:status",
        # "//mediapipe/framework/port:statusor",
        # "//mediapipe/framework/port:opencv_highgui",
        # "//mediapipe/framework/port:opencv_imgproc",
        # "//mediapipe/framework/port:opencv_video",
        # "//mediapipe/gpu:gl_calculator_helper",
        # "//mediapipe/util:resource_util",
        # "//third_party:opencv",
        "//mediapipe/gpu:gl_base",
        "//mediapipe/gpu:shader_util",
        "//third_party:opengl_glm",
        "//third_party:opengl",
        "//third_party:opengl_es",
        "//third_party:assimp",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
        "//mymediapipe/calculators/gpu:shader",
    ],
)