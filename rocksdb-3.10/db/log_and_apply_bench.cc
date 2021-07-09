//  Copyright (c) 2013, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.


#include <vector>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>
#include "thirdparty/rocksdb-3.10/util/testharness.h"
#include "thirdparty/rocksdb-3.10/util/benchharness.h"
#include "thirdparty/rocksdb-3.10/db/version_set.h"
#include "thirdparty/rocksdb-3.10/db/write_controller.h"
#include "thirdparty/rocksdb-3.10/db/writebuffer.h"
#include "thirdparty/rocksdb-3.10/util/mutexlock.h"

namespace rocksdb {

std::string MakeKey(uint64_t num) {
  char buf[30];
  snprintf(buf, sizeof(buf), "%016" PRIu64, num);
  return std::string(buf);
}

void BM_LogAndApply(int iters, int num_base_files) {
  VersionSet* vset;
  WriteController wc;
  ColumnFamilyData* default_cfd;
  uint64_t fnum = 1;
  InstrumentedMutex mu;
  InstrumentedMutexLock l(&mu);

  BENCHMARK_SUSPEND {
    std::string dbname = test::TmpDir() + "/rocksdb_test_benchmark";
    ASSERT_OK(DestroyDB(dbname, Options()));

    DB* db = nullptr;
    Options opts;
    opts.create_if_missing = true;
    Status s = DB::Open(opts, dbname, &db);
    ASSERT_OK(s);
    ASSERT_TRUE(db != nullptr);

    delete db;
    db = nullptr;

    Options options;
    EnvOptions sopt;
    // Notice we are using the default options not through SanitizeOptions().
    // We might want to initialize some options manually if needed.
    options.db_paths.emplace_back(dbname, 0);
    WriteBuffer wb(options.db_write_buffer_size);
    // The parameter of table cache is passed in as null, so any file I/O
    // operation is likely to fail.
    vset = new VersionSet(dbname, &options, sopt, nullptr, &wb, &wc);
    std::vector<ColumnFamilyDescriptor> dummy;
    dummy.push_back(ColumnFamilyDescriptor());
    ASSERT_OK(vset->Recover(dummy));
    default_cfd = vset->GetColumnFamilySet()->GetDefault();
    VersionEdit vbase;
    for (int i = 0; i < num_base_files; i++) {
      InternalKey start(MakeKey(2 * fnum), 1, kTypeValue);
      InternalKey limit(MakeKey(2 * fnum + 1), 1, kTypeDeletion);
      vbase.AddFile(2, ++fnum, 0, 1 /* file size */, start, limit, 1, 1);
    }
    ASSERT_OK(vset->LogAndApply(default_cfd,
          *default_cfd->GetLatestMutableCFOptions(), &vbase, &mu));
  }

  for (int i = 0; i < iters; i++) {
    VersionEdit vedit;
    vedit.DeleteFile(2, fnum);
    InternalKey start(MakeKey(2 * fnum), 1, kTypeValue);
    InternalKey limit(MakeKey(2 * fnum + 1), 1, kTypeDeletion);
    vedit.AddFile(2, ++fnum, 0, 1 /* file size */, start, limit, 1, 1);
    vset->LogAndApply(default_cfd, *default_cfd->GetLatestMutableCFOptions(),
                      &vedit, &mu);
  }
  delete vset;
}

BENCHMARK_NAMED_PARAM(BM_LogAndApply, 1000_iters_1_file, 1000, 1)
BENCHMARK_NAMED_PARAM(BM_LogAndApply, 1000_iters_100_files, 1000, 100)
BENCHMARK_NAMED_PARAM(BM_LogAndApply, 1000_iters_10000_files, 1000, 10000)
BENCHMARK_NAMED_PARAM(BM_LogAndApply, 100_iters_100000_files, 100, 100000)

}  // namespace rocksdb

int main(int argc, char** argv) {
  rocksdb::benchmark::RunBenchmarks();
  return 0;
}
