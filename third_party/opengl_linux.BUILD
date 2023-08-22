licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

cc_library(
    name = "opengl",
    hdrs = glob([
        # "include/**/*.h",
        # "include/GL/*.h",
        # "include/GLFW/*.h",
        # "include/glm/*.h",
    ]),
    includes = [
        # "include",
        # "include/GL",
        # "include/GLFW",
        # "include/glm",
    ],
    linkopts = [
        # "-framework OpenGL",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "glfw",
    hdrs = glob([
    ]),
    srcs = [
        # "librt.so",
        # "libm.so",
        # "libGLEW.so",
        # "libX11.so",
    ],
    visibility = ["//visibility:public"],
)
