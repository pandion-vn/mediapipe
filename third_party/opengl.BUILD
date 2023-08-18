licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

cc_library(
    name = "opengl-headers",
    hdrs = glob([
        "GL/*.h",
        "glm/*.h",
    ]),
    includes = [
        "GL/",
        "glm/"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "opengl-libs",
    srcs = [
        "librt.so",
        "libm.so",
        "libGLEW.so",
        "libX11.so",
    ],
    visibility = ["//visibility:public"],
)
