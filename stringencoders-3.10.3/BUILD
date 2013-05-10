cc_library(name = 'arraytoc',
          srcs = ['arraytoc.c',
               ],
           )

cc_library(name = 'stringencoders',
          srcs = ['modp_b85.c',
                  'modp_bjavascript.c',
                  'modp_burl.c',
                  'modp_numtoa.c',
                  'modp_ascii.c',
                  'modp_b16.cc',
                  'modp_b2.c',
                  'modp_b64.c',
                  'modp_b64w.c',
                  'modp_qsiter.c',
               ],
           )

cc_binary(name = 'modp_ascii_gen',
          srcs = ['modp_ascii_gen.c',
               ],
          deps = [':arraytoc', ],
           )

cc_binary(name = 'modp_b16_gen',
          srcs = ['modp_b16_gen.c',
               ],
          deps = [':arraytoc', ],
           )

cc_binary(name = 'modp_b2_gen',
          srcs = ['modp_b2_gen.c',
               ],
          deps = [':arraytoc', ],
           )

cc_binary(name = 'modp_b64_gen',
          srcs = ['modp_b64_gen.c',
               ],
          deps = [':arraytoc', ],
           )

cc_binary(name = 'modp_b85_gen',
          srcs = ['modp_b85_gen.c',
               ],
          deps = [':arraytoc', ],
           )

cc_binary(name = 'modp_bjavascript_gen',
          srcs = ['modp_bjavascript_gen.c',
               ],
          deps = [':arraytoc', ],
           )

cc_binary(name = 'modp_burl_gen',
          srcs = ['modp_burl_gen.c',
               ],
          deps = [':arraytoc', ],
           )
