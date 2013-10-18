cc_library(name = 'util',
    srcs = [
        'bloom.cc',
        'comparator.cc',
        'filter_policy.cc',
        'histogram.cc',
        'options.cc',
        ],
    deps = [':arena',
            ':coding',
            ':cache',
            ':crc32c',
            ':env',
            ':logging',
            ':status',
            ':hash',
            '//thirdparty/leveldb-1.9.0/port:port',
            ],
    warning = 'no',
    )

cc_library(name = 'cache',
    srcs = [
        'cache.cc',
        ],
    deps = [':hash',
            '//thirdparty/leveldb-1.9.0/port:port',
            ],
    )

cc_library(name = 'hash',
    srcs = [
        'hash.cc',
        ],
    deps = [':coding',
            ],
    )

cc_library(name = 'arena',
    srcs = [
        'arena.cc',
        ],
    warning = 'no',
#            ],
    )

cc_library(name = 'env',
    srcs = [
        'env.cc',
        'env_posix.cc',
        ],
    warning = 'no',
    deps = [':status',
            '//thirdparty/leveldb-1.9.0/port:port',
            ],
    )

cc_library(name = 'test_util',
    srcs = [
        'testharness.cc',
        'testutil.cc',
        ],
    warning = 'no',
    deps = [':env',
            ':status',
            ],
    )

cc_library(name = 'logging',
    srcs = [
        'logging.cc',
        ],
    warning = 'no',
    )

cc_library(name = 'status',
    srcs = [
        'status.cc',
        ],
    warning = 'no',
    )

cc_library(name = 'coding',
    srcs = [
        'coding.cc',
        ],
    warning = 'no',
    )

cc_library(name = 'crc32c',
    srcs = [
        'crc32c.cc',
        ],
    warning = 'no',
    )