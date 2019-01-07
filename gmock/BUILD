cc_library(
    name = 'gmock',
    deps = '//thirdparty/googletest-1.8.1/googlemock:gmock'
)

cc_library(
    name = 'gmock_main',
    deps = '//thirdparty/googletest-1.8.1/googlemock:gmock_main'
)

cc_test(
    name = 'gmock_test',
    srcs = 'gmock_test.cpp',
    deps = [':gmock']
)
