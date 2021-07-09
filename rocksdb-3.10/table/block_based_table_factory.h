//  Copyright (c) 2013, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once
#include <stdint.h>

#include <memory>
#include <string>

#include "thirdparty/rocksdb-3.10/include/rocksdb/flush_block_policy.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/table.h"
#include "thirdparty/rocksdb-3.10/db/dbformat.h"

namespace rocksdb {

struct EnvOptions;

using std::unique_ptr;
class BlockBasedTableBuilder;

class BlockBasedTableFactory : public TableFactory {
 public:
  explicit BlockBasedTableFactory(
      const BlockBasedTableOptions& table_options = BlockBasedTableOptions());

  ~BlockBasedTableFactory() {}

  const char* Name() const override { return "BlockBasedTable"; }

  Status NewTableReader(const ImmutableCFOptions& ioptions,
                        const EnvOptions& soptions,
                        const InternalKeyComparator& internal_comparator,
                        unique_ptr<RandomAccessFile>&& file, uint64_t file_size,
                        unique_ptr<TableReader>* table_reader) const override {
    return NewTableReader(ioptions, soptions, internal_comparator,
                          std::move(file), file_size, table_reader,
                          /*prefetch_index_and_filter=*/true);
  }

  // This is a variant of virtual member function NewTableReader function with
  // added capability to disable pre-fetching of blocks on BlockBasedTable::Open
  Status NewTableReader(const ImmutableCFOptions& ioptions,
                        const EnvOptions& soptions,
                        const InternalKeyComparator& internal_comparator,
                        unique_ptr<RandomAccessFile>&& file, uint64_t file_size,
                        unique_ptr<TableReader>* table_reader,
                        bool prefetch_index_and_filter) const;

  TableBuilder* NewTableBuilder(
      const ImmutableCFOptions& ioptions,
      const InternalKeyComparator& internal_comparator, WritableFile* file,
      const CompressionType compression_type,
      const CompressionOptions& compression_opts,
      const bool skip_filters = false) const override;

  // Sanitizes the specified DB Options.
  Status SanitizeOptions(const DBOptions& db_opts,
                         const ColumnFamilyOptions& cf_opts) const override;

  std::string GetPrintableTableOptions() const override;

  const BlockBasedTableOptions& GetTableOptions() const;

 private:
  BlockBasedTableOptions table_options_;
};

extern const std::string kHashIndexPrefixesBlock;
extern const std::string kHashIndexPrefixesMetadataBlock;
extern const std::string kPropTrue;
extern const std::string kPropFalse;

}  // namespace rocksdb
