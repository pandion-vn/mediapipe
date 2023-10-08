licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

cc_library(
    name = "assimp_linux_x86_64",
    hdrs = glob([
      "include/assimp/*.h*",
      "include/assimp/*.inl",
      "include/assimp/**/*.h*",
      "contrib/utf8cpp/**/*.h*",
    ]),
    srcs = glob([
      "lib/linux_x86_64/libassimp.a",    
    ]),
    linkopts = [
        "-lGL",
        "-ldl",
        "-lm",
        "-lz",
        "-lpthread",
        "-lrt",
    ],
    includes = [
      "include/",
      "contrib/",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "assimp_linux_aarch64",
    hdrs = glob([
      "include/assimp/*.h*",
      "include/assimp/**/*.h*",
    ]),
    srcs = glob([
      "lib/linux_aarch64/libassimp.a",    
    ]),
    linkopts = [
        "-lGL",
        "-ldl",
        "-lm",
        "-lz",
        "-lpthread",
        "-lrt",
    ],
    includes = [
      "include/",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)