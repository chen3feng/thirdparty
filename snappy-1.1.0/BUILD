cc_library(name = 'snappy',
     srcs = [
        'snappy.cc',
        'snappy-c.cc',
        'snappy-sinksource.cc',
        'snappy-stubs-internal.cc'
    ],
    warning = 'no',
)

cc_binary(name = 'snappy_unittest',
     srcs = [
        'snappy_unittest.cc',
        'snappy-test.cc',
    ],
    #FIXME(yeshunping) : Is it a good idea to treat libz as system library ?
    deps = [':snappy', '#z'],
    warning = 'no',
)