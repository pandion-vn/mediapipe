licenses(["notice"])

package(default_visibility = [
    "//visibility:public",
])

cc_library(
    name = "simple_opengl",
    srcs = ["main.cc"],
    deps = [
        # "//third_party:opencv",
        "//third_party:opengl",
        "//third_party:opengl_glfw",
        # "@system_headers//:headers",
        # "@system_libs//:opengl",
        # "@effekseer_libs//:glfw-dev",
        # "@effekseer_libs//:effekseer-dev",
    ],
    data = [
        # "//myMediapipe/examples/desktop/assets:Laser01.efk",
        # "//myMediapipe/examples/desktop/assets:Laser03.efk",
        # "//myMediapipe/examples/desktop/assets:SimpleFragmentShader.fragmentshader",
        # "//myMediapipe/examples/desktop/assets:SimpleVertexShader.vertexshader",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "simple_opengl_demo",
    deps = [
        "//mymediapipe/examples/desktop/simple_opengl:simple_opengl",
        # "//mymediapipe/graphs/simple_io:desktop_simple_io_gpu_calculators",
    ],
)