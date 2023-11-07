load(
    "//mediapipe/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ik_opengl_deps",
    deps = [
        # "//mediapipe/calculators/core:constant_side_packet_calculator",
        # "//mediapipe/calculators/core:flow_limiter_calculator",
        # "//mediapipe/graphs/pose_tracking/subgraphs:pose_renderer_gpu",
        # "//mediapipe/modules/pose_landmark:pose_landmark_gpu",
        # "//mymediapipe/calculators/debugger:common_debugger_calculator",
        # "//mediapipe/modules/face_geometry:env_generator_calculator",
        # "//mymediapipe/graphs/glanimation/calculators:gl_11b_coordinate_system_calculator",
        # "//mymediapipe/graphs/glanimation/calculators:gl_18_camera_skeletal_animation_calculator",
        "//mymediapipe/graphs/ik_opengl/calculators:gl_ik_opengl_calculator",
    ],
)

cc_library(
    name = "ik_opengl_pose_deps",
    deps = [
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/graphs/pose_tracking/subgraphs:pose_renderer_gpu",
        "//mediapipe/modules/pose_landmark:pose_landmark_gpu",
        "//mymediapipe/graphs/ik_opengl/calculators:gl_ik_opengl_pose_calculator",
        # "//mymediapipe/calculators/debugger:common_debugger_calculator",
        # "//mediapipe/modules/face_geometry:env_generator_calculator",
        # "//mymediapipe/graphs/glanimation/calculators:gl_11b_coordinate_system_calculator",
        # "//mymediapipe/graphs/glanimation/calculators:gl_18_camera_skeletal_animation_calculator",
        # "//mymediapipe/graphs/glanimation/calculators:gl_18a_camera_skeletal_animation_calculator",
    ],
)