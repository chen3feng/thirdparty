cc_library(
    name = 'gmock',
    srcs = [
        'src/gmock-cardinalities.cc',
        'src/gmock.cc',
        'src/gmock-internal-utils.cc',
        'src/gmock-matchers.cc',
        'src/gmock-spec-builders.cc'
    ],
    incs = ['include', '../googletest/include'],
    deps = '//thirdparty/gtest:gtest',
    visibility = ['//thirdparty/gmock:gmock'],
)

cc_library(
    name = 'gmock_main',
    srcs = [
        'src/gmock_main.cc',
    ],
    incs = ['include', '../googletest/include'],
    deps = ':gmock',
    visibility = ['//thirdparty/gmock:gmock_main'],
)
