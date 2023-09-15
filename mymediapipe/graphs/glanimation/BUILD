load(
    "//mediapipe/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

exports_files(glob([
    "*.pbtxt",
]))

cc_library(
    name = "glanimation_calculators",
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/image:image_cropping_calculator",
        # "//mediapipe/graphs/object_detection_3d/calculators:annotations_to_model_matrices_calculator",
        "//mediapipe/graphs/object_detection_3d/calculators:gl_animation_overlay_calculator",
        # "//mediapipe/modules/objectron:objectron_gpu",
    ],
)

cc_library(
    name = "shader_calculators",
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/image:image_cropping_calculator",
        "//mymediapipe/graphs/glanimation/calculators:lens_flare_shader_calculator",
        "//mymediapipe/graphs/glanimation/calculators:triangle_shader_calculator",
        # "//mediapipe/graphs/object_detection_3d/calculators:annotations_to_model_matrices_calculator",
        # "//mediapipe/graphs/object_detection_3d/calculators:gl_animation_overlay_calculator",
        # "//mediapipe/modules/objectron:objectron_gpu",
    ],
)
