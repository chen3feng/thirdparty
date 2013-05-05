cc_library(name = 'skiplist',
           deps = [
                '//thirdparty/leveldb-1.9.0/util:arena',
                '//thirdparty/leveldb-1.9.0/util:hash',
                '//thirdparty/leveldb-1.9.0/util:env',
                ],
           )

cc_binary(name = 'skiplist_test',
          srcs = ['skiplist_test.cc',
                  ],
          deps = [
                ':skiplist',
                '//thirdparty/leveldb-1.9.0/util:test_util',
                ],
          warning = 'no',
          )