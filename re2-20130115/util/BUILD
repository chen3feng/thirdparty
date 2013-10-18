cc_library(
    name = 'arena',
    srcs = [
        'arena.cc',
    ],
    deps = ['//thirdparty/gflags:gflags',
            '//thirdparty/glog:glog',
            ],
    warning = 'no'
)

cc_library(
    name = 'utf',
    srcs = [
        'rune.cc',
    ],
    deps = ['//thirdparty/gflags:gflags',
            '//thirdparty/glog:glog',
            ],
    warning = 'no'
)

cc_library(
    name = 'pcre',
    srcs = [
        'pcre.cc',
    ],
    deps = [':string_util',
            '//thirdparty/gflags:gflags',
            '//thirdparty/glog:glog',
            ],
    warning = 'no'
)

cc_library(
    name = 'random',
    srcs = [
        'random.cc',
    ],
    deps = ['//thirdparty/gflags:gflags',
            '//thirdparty/glog:glog',
            ],
    warning = 'no'
)

cc_library(
    name = 'mini_test',
    srcs = [
        'test.cc',
    ],
    deps = ['//thirdparty/gflags:gflags',
            '//thirdparty/glog:glog',
            ],
    warning = 'no'
)

cc_library(
    name = 'string_util',
    srcs = [
        'stringpiece.cc',
        'stringprintf.cc',
        'strutil.cc',
    ],
    deps = ['//thirdparty/gflags:gflags',
            '//thirdparty/glog:glog',
            ],
    warning = 'no'
)

cc_library(
    name = 'util',
    srcs = [
        'hash.cc',
        'thread.cc',
        'valgrind.cc'
    ],
    deps = [':arena',
            ':utf',
            ':pcre',
            ':random',
            ':string_util',
            '//thirdparty/gflags:gflags',
            '//thirdparty/glog:glog',
            ],
    warning = 'no'
)

cc_library(name = 'benchmark',
           srcs = ['benchmark.cc',],
           deps = [
            '//thirdparty/gflags:gflags',
            '//thirdparty/glog:glog',
            '//thirdparty/perftools:tcmalloc_and_profiler',
            ],
           )

cc_binary(name = 're2_benchmark',
          srcs = ['re2_benchmark.cc',],
          deps = [
            ':benchmark',
            '//thirdparty/re2-20130115/re2:re2',
            '//thirdparty/gflags:gflags',
            '//thirdparty/glog:glog',
            ],
           )