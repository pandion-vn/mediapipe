licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

cc_library(
    name = "opengl_glm",
    hdrs = glob([
      "glm/*.h*",
      "glm/**/*.h*",
      "glm/**/*.inl",
    ]),
    includes = [
      "glm/",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)