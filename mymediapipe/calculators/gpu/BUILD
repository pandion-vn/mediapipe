licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:private"])

exports_files(["LICENSE"])

load("//mediapipe/framework/port:build_config.bzl", "mediapipe_cc_proto_library")

cc_library(
    name = "framebuffer",
    visibility = ["//visibility:public"],
    srcs = ["framebuffer_target.h"],
    hdrs = ["framebuffer_target.h"],
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
        # "//third_party:opengl",
        # "//third_party:opengl_es",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
        "//mediapipe/gpu:shader_util",
    ],
)

cc_library(
    name = "video_scene",
    visibility = ["//visibility:public"],
    srcs = ["video_scene.h"],
    hdrs = ["video_scene.h"],
    deps = [
        "//mediapipe/gpu:shader_util",
    ],
)

cc_library(
    name = "shader",
    visibility = ["//visibility:public"],
    srcs = ["shader.h"],
    hdrs = ["shader.h"],
    deps = [
        "//mediapipe/gpu:shader_util",
    ],
)
