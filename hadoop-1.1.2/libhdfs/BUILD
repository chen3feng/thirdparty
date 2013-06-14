cc_library(
    name = 'hdfs',
    srcs = [
        'hdfs.c',
        'hdfsJniHelper.c',
    ],
    #FIXME(yeshunping) : how to set path for it properly?
    incs = ['/usr/lib/jvm/java-7-openjdk-amd64/include/', '.'],
    warning = 'no',
)

#FIXME(yeshunping) : For linking, libjvm is needed, but it has only shared
# library, what should I do ?
cc_binary(
    name = 'hdfs_read',
    srcs = [
        'hdfs_read.c',
    ],
    deps = [':hdfs'],
    incs = ['/usr/lib/jvm/java-7-openjdk-amd64/include/', '.'],
    warning = 'no',
)

cc_binary(
    name = 'hdfs_write',
    srcs = [
        'hdfs_write.c'
    ],
    deps = [':hdfs'],
    incs = ['/usr/lib/jvm/java-7-openjdk-amd64/include/', '.'],
    warning = 'no',
)
