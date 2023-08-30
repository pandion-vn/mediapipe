licenses(["notice"])

package(default_visibility = [
    "//visibility:public",
])

cc_library(
    name = "demo_run_graph_main_gpu",
    srcs = ["main.cc"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/framework/port:opencv_video",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        "//mediapipe/gpu:gl_calculator_helper",
        # "//mediapipe/gpu:gpu_buffer",
        # "//mediapipe/gpu:gpu_shared_data_internal",
        "//mediapipe/util:resource_util",
        "//third_party:opencv",
        "//third_party:opengl",
        "//third_party:opengl_glew",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
    linkopts = [
        # "-lGLESv2",
    ],
)

cc_binary(
    name = "simple_egl_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_egl:demo_run_graph_main_gpu",
        "//mymediapipe/graphs/simple_io:desktop_simple_io_gpu_calculators",
    ],
)