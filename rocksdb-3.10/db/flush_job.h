//  Copyright (c) 2013, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
#pragma once

#include <atomic>
#include <deque>
#include <limits>
#include <set>
#include <utility>
#include <vector>
#include <string>

#include "thirdparty/rocksdb-3.10/db/dbformat.h"
#include "thirdparty/rocksdb-3.10/db/log_writer.h"
#include "thirdparty/rocksdb-3.10/db/snapshot.h"
#include "thirdparty/rocksdb-3.10/db/column_family.h"
#include "thirdparty/rocksdb-3.10/db/version_edit.h"
#include "thirdparty/rocksdb-3.10/db/memtable_list.h"
#include "thirdparty/rocksdb-3.10/port/port.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/db.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/env.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/memtablerep.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/transaction_log.h"
#include "thirdparty/rocksdb-3.10/util/autovector.h"
#include "thirdparty/rocksdb-3.10/util/event_logger.h"
#include "thirdparty/rocksdb-3.10/util/instrumented_mutex.h"
#include "thirdparty/rocksdb-3.10/util/stop_watch.h"
#include "thirdparty/rocksdb-3.10/util/thread_local.h"
#include "thirdparty/rocksdb-3.10/util/scoped_arena_iterator.h"
#include "thirdparty/rocksdb-3.10/db/internal_stats.h"
#include "thirdparty/rocksdb-3.10/db/write_controller.h"
#include "thirdparty/rocksdb-3.10/db/flush_scheduler.h"
#include "thirdparty/rocksdb-3.10/db/write_thread.h"
#include "thirdparty/rocksdb-3.10/db/job_context.h"

namespace rocksdb {

class MemTable;
class TableCache;
class Version;
class VersionEdit;
class VersionSet;
class Arena;

class FlushJob {
 public:
  // TODO(icanadi) make effort to reduce number of parameters here
  // IMPORTANT: mutable_cf_options needs to be alive while FlushJob is alive
  FlushJob(const std::string& dbname, ColumnFamilyData* cfd,
           const DBOptions& db_options,
           const MutableCFOptions& mutable_cf_options,
           const EnvOptions& env_options, VersionSet* versions,
           InstrumentedMutex* db_mutex, std::atomic<bool>* shutting_down,
           SequenceNumber newest_snapshot, JobContext* job_context,
           LogBuffer* log_buffer, Directory* db_directory,
           Directory* output_file_directory, CompressionType output_compression,
           Statistics* stats, EventLogger* event_logger);

  ~FlushJob();

  Status Run(uint64_t* file_number = nullptr);

 private:
  Status WriteLevel0Table(const autovector<MemTable*>& mems, VersionEdit* edit,
                          uint64_t* filenumber);
  const std::string& dbname_;
  ColumnFamilyData* cfd_;
  const DBOptions& db_options_;
  const MutableCFOptions& mutable_cf_options_;
  const EnvOptions& env_options_;
  VersionSet* versions_;
  InstrumentedMutex* db_mutex_;
  std::atomic<bool>* shutting_down_;
  SequenceNumber newest_snapshot_;
  JobContext* job_context_;
  LogBuffer* log_buffer_;
  Directory* db_directory_;
  Directory* output_file_directory_;
  CompressionType output_compression_;
  Statistics* stats_;
  EventLogger* event_logger_;
};

}  // namespace rocksdb
