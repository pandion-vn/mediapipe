licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:private"])

exports_files(["LICENSE"])

load("//mediapipe/framework/port:build_config.bzl", "mediapipe_cc_proto_library")

cc_library(
    name = "ik_calculators",
    visibility = ["//visibility:public"],
    srcs = [
        "target.cc",
        "mesh.cc",
        "segment.cc",
        "chain.cc",
        "model.cc",
    ],
    hdrs = [
        "camera.h",
        "mesh.h",
        # "shader.h",
        "target.h",
        "segment.h",
        "chain.h",
        "model.h",
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
