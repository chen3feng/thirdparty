cc_library(
    name = 'arraytoc',
    srcs = [
        'src/arraytoc.c',
    ],
)

cc_library(
    name = 'stringencoders',
    srcs = [
        'src/modp_ascii.c',
        'src/modp_b16.c',
        'src/modp_b2.c',
        'src/modp_b64.c',
        'src/modp_b64r.c',
        'src/modp_b64w.c',
        'src/modp_b85.c',
        'src/modp_bjavascript.c',
        'src/modp_burl.c',
        'src/modp_numtoa.c',
        'src/modp_qsiter.c',
        'src/modp_xml.c'
    ],
    extra_cppflags = '-Wno-float-equal'
)

cc_binary(
    name = 'modp_ascii_gen',
    srcs = ['src/modp_ascii_gen.c'],
    deps = [':arraytoc'],
)

cc_binary(
    name = 'modp_b16_gen',
    srcs = ['src/modp_b16_gen.c'],
    deps = [':arraytoc'],
)

cc_binary(
    name = 'modp_b2_gen',
    srcs = ['src/modp_b2_gen.c'],
    deps = [':arraytoc'],
)

cc_binary(
    name = 'modp_b64_gen',
    srcs = ['src/modp_b64_gen.c'],
    deps = [':arraytoc'],
)

cc_binary(
    name = 'modp_b85_gen',
    srcs = ['src/modp_b85_gen.c'],
    deps = [':arraytoc'],
)

cc_binary(
    name = 'modp_bjavascript_gen',
    srcs = ['src/modp_bjavascript_gen.c'],
    deps = [':arraytoc'],
)

cc_binary(
    name = 'modp_burl_gen',
    srcs = ['src/modp_burl_gen.c'],
    deps = [':arraytoc', ],
)
