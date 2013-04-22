cc_library(name = 'profiler',
           deps = '//thirdparty/gperftools-2.0/src:profiler')
cc_library(name = 'tcmalloc',
           deps = '//thirdparty/gperftools-2.0/src:tcmalloc')
cc_library(name = 'tcmalloc_debug',
           deps = '//thirdparty/gperftools-2.0/src:tcmalloc_debug')
cc_library(name = 'tcmalloc_and_profiler',
           deps = '//thirdparty/gperftools-2.0/src:tcmalloc_and_profiler')
cc_library(name = 'tcmalloc_minimal',
           deps = '//thirdparty/gperftools-2.0/src:tcmalloc_minimal')


cc_test(
    name = 'tcmalloc_test',
    srcs = 'tcmalloc_test.cpp',
    deps = ':tcmalloc_debug',
)
