// Copyright 2009 The RE2 Authors.  All Rights Reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "thirdparty/re2-20130115/util/util.h"
#include "thirdparty/re2-20130115/util/valgrind.h"

namespace re2 {

int RunningOnValgrind() {
#ifdef RUNNING_ON_VALGRIND
	return RUNNING_ON_VALGRIND;
#else
	return 0;
#endif
}

}  // namespace re2
