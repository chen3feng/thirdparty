cc_library(
  name = 'filesystem',
  srcs = [
    'libs/filesystem/src/codecvt_error_category.cpp',
    'libs/filesystem/src/operations.cpp',
    'libs/filesystem/src/path.cpp',
    'libs/filesystem/src/path_traits.cpp',
    'libs/filesystem/src/portability.cpp',
    'libs/filesystem/src/unique_path.cpp',
    'libs/filesystem/src/utf8_codecvt_facet.cpp',
  ],
  deps = [
    ':system',
  ],
  warning = 'no',
)

cc_library(
  name = 'system',
  srcs = [
    'libs/system/src/error_code.cpp'
  ],
  warning = 'no',
)

cc_library(
  name = 'thread',
  incs = '.',
  srcs = [
    'libs/thread/src/pthread/thread.cpp',
    'libs/thread/src/pthread/once.cpp',
    'libs/thread/src/pthread/once_atomic.cpp',
  ],
  deps = [
    '#pthread',
  ],
)

cc_library(
  name = 'date_time',
  srcs = [
    'libs/date_time/src/gregorian/date_generators.cpp',
    'libs/date_time/src/gregorian/greg_weekday.cpp',
    'libs/date_time/src/gregorian/greg_month.cpp',
    'libs/date_time/src/gregorian/gregorian_types.cpp',
    'libs/date_time/src/posix_time/posix_time_types.cpp',
  ],
)
