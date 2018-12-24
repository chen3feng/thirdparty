'''
licenses(["notice"])

config_setting(
    name = "windows",
    values = {
        "cpu": "x64_windows",
    },
    visibility = [":__subpackages__"],
)
'''

cc_library(
    name = "benchmark",
    srcs = glob(
        [
            "src/*.cc",
#            "src/*.h",
        ],
        excludes = ["src/benchmark_main.cc"],
    ),
#    hdrs = ["include/benchmark/benchmark.h"],
#    linkopts = select({
#        ":windows": ["-DEFAULTLIB:shlwapi.lib"],
#        "//conditions:default": ["-pthread"],
#    }),
#    strip_include_prefix = "include",
    deps = ['#pthread'],
    export_incs = ['include'],
    visibility = ["//thirdparty/benchmark:benchmark"],
)

cc_library(
    name = "benchmark_main",
    srcs = ["src/benchmark_main.cc"],
#    hdrs = ["include/benchmark/benchmark.h"],
#    strip_include_prefix = "include",
    deps = [":benchmark"],
    visibility = ["//thirdparty/benchmark:benchmark_main"],
)
