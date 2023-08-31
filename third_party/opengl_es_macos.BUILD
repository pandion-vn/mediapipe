licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

cc_library(
    name = "opengl_es",
    hdrs = glob([
      "include/EGL/*.h",
      "include/GLES2/*.h",
    ]),
    srcs = glob([
      "lib/macOS_x86_64/libEGL.dylib",
      "lib/macOS_x86_64/libGLESv1_CM.dylib",
      "lib/macOS_x86_64/libGLESv2.dylib",
    ]),
    linkstatic = 1,
    visibility = ["//visibility:public"],
)