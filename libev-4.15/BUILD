cc_library(
    name = 'ev',
    srcs = 'ev.c',
    extra_cppflags = [
        '-Wno-sign-compare',
        '-Wno-strict-aliasing',
        '-Wno-float-equal',
        '-Wno-unused-result',
        '-Wno-unused-value',
    ],
)

cc_library(
    name = 'event',
    srcs = 'event.c',
    extra_cppflags = [
        '-Wno-strict-aliasing',
        '-Wno-float-equal',
    ],
    deps = ':ev',
)
