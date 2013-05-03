// Copyright 2009 The RE2 Authors.  All Rights Reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Simplified version of Google's logging.

#ifndef RE2_UTIL_LOGGING_H__
#define RE2_UTIL_LOGGING_H__

#include <unistd.h>  /* for write */
#include <sstream>
#include "thirdparty/glog/logging.h"

#ifdef NDEBUG
#define DEBUG_MODE 0
#define LOG_DFATAL LOG_ERROR
#else
#define DEBUG_MODE 1
#define LOG_DFATAL LOG_FATAL
#endif

#endif  // RE2_UTIL_LOGGING_H__
