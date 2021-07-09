// Copyright (c) 2013, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.
#ifndef ROCKSDB_LITE
#pragma once

#include "thirdparty/rocksdb-3.10/include/rocksdb/sst_dump_tool.h"

#include <map>
#include <string>
#include <vector>

#include "thirdparty/rocksdb-3.10/db/dbformat.h"
#include "thirdparty/rocksdb-3.10/db/memtable.h"
#include "thirdparty/rocksdb-3.10/db/write_batch_internal.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/db.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/env.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/immutable_options.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/iterator.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/slice_transform.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/status.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/table.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/table_properties.h"
#include "thirdparty/rocksdb-3.10/table/block.h"
#include "thirdparty/rocksdb-3.10/table/block_based_table_factory.h"
#include "thirdparty/rocksdb-3.10/table/block_builder.h"
#include "thirdparty/rocksdb-3.10/table/format.h"
#include "thirdparty/rocksdb-3.10/table/meta_blocks.h"
#include "thirdparty/rocksdb-3.10/table/plain_table_factory.h"
#include "thirdparty/rocksdb-3.10/util/ldb_cmd.h"
#include "thirdparty/rocksdb-3.10/util/random.h"
#include "thirdparty/rocksdb-3.10/util/testharness.h"
#include "thirdparty/rocksdb-3.10/util/testutil.h"

namespace rocksdb {

class SstFileReader {
 public:
  explicit SstFileReader(const std::string& file_name, bool verify_checksum,
                         bool output_hex);

  Status ReadSequential(bool print_kv, uint64_t read_num, bool has_from,
                        const std::string& from_key, bool has_to,
                        const std::string& to_key);

  Status ReadTableProperties(
      std::shared_ptr<const TableProperties>* table_properties);
  uint64_t GetReadNumber() { return read_num_; }
  TableProperties* GetInitTableProperties() { return table_properties_.get(); }

  Status DumpTable(const std::string& out_filename);
  Status getStatus() { return init_result_; }

 private:
  // Get the TableReader implementation for the sst file
  Status GetTableReader(const std::string& file_path);
  Status ReadTableProperties(uint64_t table_magic_number,
                             RandomAccessFile* file, uint64_t file_size);
  Status SetTableOptionsByMagicNumber(uint64_t table_magic_number);
  Status SetOldTableOptions();

  // Helper function to call the factory with settings specific to the
  // factory implementation
  Status NewTableReader(const ImmutableCFOptions& ioptions,
                        const EnvOptions& soptions,
                        const InternalKeyComparator& internal_comparator,
                        unique_ptr<RandomAccessFile>&& file, uint64_t file_size,
                        unique_ptr<TableReader>* table_reader);

  std::string file_name_;
  uint64_t read_num_;
  bool verify_checksum_;
  bool output_hex_;
  EnvOptions soptions_;

  Status init_result_;
  unique_ptr<TableReader> table_reader_;
  unique_ptr<RandomAccessFile> file_;
  // options_ and internal_comparator_ will also be used in
  // ReadSequential internally (specifically, seek-related operations)
  Options options_;
  const ImmutableCFOptions ioptions_;
  InternalKeyComparator internal_comparator_;
  unique_ptr<TableProperties> table_properties_;
};

}  // namespace rocksdb

#endif  // ROCKSDB_LITE
