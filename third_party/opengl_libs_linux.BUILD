licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

cc_library(
    name = "opengl_glfw",
    hdrs = glob([
      "include/GLFW/*.h",
    ]),
    srcs = glob([
      "lib/libglfw3.a",    
    ]),
    linkopts = [
        "-lGL",
        "-ldl",
        "-lm",
        "-lpthread",
        "-lrt",
    ],
    includes = [
      "include/GLFW/",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "opengl_glew",
    hdrs = glob([
      "include/GL/*.h",
    ]),
    srcs = glob([
      "lib/libGLEW.a",
    ]),
    linkopts = [
        "-lGL",
        "-ldl",
        "-lm",
        "-lpthread",
        "-lrt",
        "-l:libGLEW.so",
    ],
    includes = [
      "include/GL/",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)