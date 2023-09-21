licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "simple_shader",
    hdrs = [
    ],
    srcs = [
        "main.cc",
    ],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:map_util",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:ret_check",
        "//mediapipe/framework/port:status",
        "//mediapipe/framework/port:statusor",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/framework/port:opencv_video",
        "//mediapipe/gpu:gl_calculator_helper",
        "//mediapipe/util:resource_util",
        "//third_party:opencv",
        # "//third_party:opengl",
        # "//third_party:opengl_es",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
    ],
)

cc_binary(
    name = "shader_lens_flare_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_shader:simple_shader",
        "//mymediapipe/graphs/glanimation:shader_calculators",
    ],
)

cc_binary(
    name = "gl_shader_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_shader:simple_shader",
        "//mymediapipe/graphs/glanimation:gl_shader_calculators",
    ],
)

cc_binary(
    name = "opengl_shader_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_shader:simple_shader",
        "//mymediapipe/graphs/glanimation:opengl_shader_calculators",
    ],
)