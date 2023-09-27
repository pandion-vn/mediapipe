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

cc_library(
    name = "gl_shader_calculators",
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/image:image_cropping_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_shader_calculator",
    ],
)


cc_library(
    name = "opengl_shader_calculators",
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/image:image_cropping_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_02_dot_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_03_triangle_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_05_triangle_uniform_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_06_triangle_transformation_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_09_triangle_interpolation_calculator",
        "//mymediapipe/graphs/glanimation/calculators:gl_10_triangle_texture_calculator",
        "//mymediapipe/graphs/glanimation/calculators:opengl_shader_calculator",
    ],
)
