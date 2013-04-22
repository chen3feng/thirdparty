cc_library(
    name = 'logging',
    srcs = [
        'base/logging.cc',
        'base/dynamic_annotations.c'
    ],
    deps = [
        '//thirdparty/gflags:gflags'
    ],
    incs = '.',
    warning = 'no'
)

cc_library(
    name = 'sysinfo',
    srcs = 'base/sysinfo.cc',
    deps = ':logging',
    incs = '.',
    warning = 'no'
)

cc_library(
    name = 'spinlock',
    srcs = [
        'base/spinlock.cc',
        'base/spinlock_internal.cc',
        'base/atomicops-internals-x86.cc'
    ],
    extra_cppflags = enable_if(build_target.arch == 'i386', '-mmmx'),
    deps = ':sysinfo',
    incs = '.',
    warning = 'no'
)

cc_library(
    name = 'stacktrace',
    srcs = [
        'stacktrace.cc',
        'base/elf_mem_image.cc',
        'base/vdso_support.cc'
    ],
    deps = [
        ':spinlock',
    ],
    incs = '.',
    warning = 'no'
)

cc_library(
    name = 'profiler',
    srcs = [
        'profiler.cc',
        'profile-handler.cc',
        'profiledata.cc',
    ],
    deps = [
        ':stacktrace',
    ],
    incs = '.',
    warning = 'no',
    link_all_symbols = True
)

cc_library(
    name = 'tcmalloc_internal',
    srcs = [
        'common.cc',
        'internal_logging.cc',
        'system-alloc.cc',
        'memfs_malloc.cc',
        'central_freelist.cc',
        'page_heap.cc',
        'sampler.cc',
        'span.cc',
        'stack_trace_table.cc',
        'static_vars.cc',
        'symbolize.cc',
        'thread_cache.cc',
        'malloc_hook.cc',
        'malloc_extension.cc',
        'maybe_threads.cc',
        'base/thread_lister.c',
        'base/linuxthreads.cc',
        'heap-checker.cc',
        'heap-checker-bcad.cc',
        'base/low_level_alloc.cc',
        'heap-profile-table.cc',
        'raw_printer.cc',
        'memory_region_map.cc'
    ],
    deps = [
        '#pthread',
        ':stacktrace'
    ],
    incs = '.',
    warning = 'no',
    link_all_symbols = True
)

cc_library(
    name = 'tcmalloc',
    srcs = [
        'heap-profiler.cc'
    ],
    deps = [
        ':tcmalloc_minimal',
    ],
    incs = '.',
    warning = 'no',
    link_all_symbols = True
)

cc_library(
    name = 'tcmalloc_debug',
    srcs = 'debugallocation.cc',
    deps = [
        ':tcmalloc_internal',
        '//thirdparty/glog:glog'
    ],
    incs = '.',
    defs = 'TCMALLOC_FOR_DEBUGALLOCATION',
    warning = 'no'
)

cc_library(
    name = 'tcmalloc_and_profiler',
    deps = [
        ':profiler',
        ':tcmalloc'
    ]
)

cc_library(
    name = 'tcmalloc_minimal',
    srcs = [
        'tcmalloc.cc',
    ],
    deps = ':tcmalloc_internal',
    incs = '.',
    warning = 'no'
)

