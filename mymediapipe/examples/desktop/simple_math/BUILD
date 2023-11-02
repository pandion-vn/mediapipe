licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "glm_deps",
    hdrs = [
        "my_math.h",
        "my_pose.h",
    ],
    srcs = [
        "my_math.h",
        "my_pose.h",
        "main.cc",
    ],
    deps = [
        "//third_party:opengl_glm",
        # "//mediapipe/framework:calculator_framework",
        # "//mediapipe/framework/formats:image_frame",
        # "//mediapipe/framework/formats:image_frame_opencv",
        # "//mediapipe/framework/port:file_helpers",
        # "//mediapipe/framework/port:mmap_util",
        # "//mediapipe/framework/port:parse_text_proto",
        # "//mediapipe/framework/port:ret_check",
        # "//mediapipe/framework/port:status",
        # "//mediapipe/framework/port:statusor",
        # "//mediapipe/framework/port:opencv_highgui",
        # "//mediapipe/framework/port:opencv_imgproc",
        # "//mediapipe/framework/port:opencv_video",
        # "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/util:resource_util",
        "//third_party:opencv",
        "@com_google_absl//absl/memory",
        "@eigen_archive//:eigen3",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings"
    ],
)

cc_binary(
    name = "simple_math_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_math:glm_deps",
    ],
)