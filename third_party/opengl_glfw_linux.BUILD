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