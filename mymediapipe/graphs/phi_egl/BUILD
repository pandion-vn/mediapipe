load(
    "//mediapipe/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "phi_egl_deps",
    deps = [
        "//mymediapipe/graphs/phi_egl/calculators:phi_ani_egl_calculator",
        "//mymediapipe/graphs/phi_egl/calculators:phi_egl_calculator",
    ],
)

cc_library(
    name = "phi_egl_pose_deps",
    deps = [
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/graphs/pose_tracking/subgraphs:pose_renderer_gpu",
        "//mediapipe/modules/pose_landmark:pose_landmark_gpu",
        # "//mymediapipe/graphs/phi_egl/calculators:gl_ik_opengl_pose_calculator",
        # "//mymediapipe/calculators/debugger:common_debugger_calculator",
        # "//mediapipe/modules/face_geometry:env_generator_calculator",
        # "//mymediapipe/graphs/glanimation/calculators:gl_11b_coordinate_system_calculator",
        # "//mymediapipe/graphs/glanimation/calculators:gl_18_camera_skeletal_animation_calculator",
        # "//mymediapipe/graphs/glanimation/calculators:gl_18a_camera_skeletal_animation_calculator",
    ],
)