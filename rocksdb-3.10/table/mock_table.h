// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//  Copyright (c) 2013, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
#pragma once
#include <algorithm>
#include <set>
#include <memory>
#include <atomic>
#include <map>
#include <string>

#include "thirdparty/rocksdb-3.10/include/rocksdb/table.h"
#include "thirdparty/rocksdb-3.10/table/table_reader.h"
#include "thirdparty/rocksdb-3.10/table/table_builder.h"
#include "thirdparty/rocksdb-3.10/port/port.h"
#include "thirdparty/rocksdb-3.10/util/mutexlock.h"
#include "thirdparty/rocksdb-3.10/util/testharness.h"
#include "thirdparty/rocksdb-3.10/util/testutil.h"

namespace rocksdb {
namespace mock {

typedef std::map<std::string, std::string> MockFileContents;
// NOTE this currently only supports bitwise comparator

struct MockTableFileSystem {
  port::Mutex mutex;
  std::map<uint32_t, MockFileContents> files;
};

class MockTableReader : public TableReader {
 public:
  explicit MockTableReader(const MockFileContents& table) : table_(table) {}

  Iterator* NewIterator(const ReadOptions&, Arena* arena) override;

  Status Get(const ReadOptions&, const Slice& key,
             GetContext* get_context) override;

  uint64_t ApproximateOffsetOf(const Slice& key) override { return 0; }

  virtual size_t ApproximateMemoryUsage() const override { return 0; }

  void SetupForCompaction() override {}

  std::shared_ptr<const TableProperties> GetTableProperties() const override;

  ~MockTableReader() {}

 private:
  const MockFileContents& table_;
};

class MockTableIterator : public Iterator {
 public:
  explicit MockTableIterator(const MockFileContents& table) : table_(table) {
    itr_ = table_.end();
  }

  bool Valid() const override { return itr_ != table_.end(); }

  void SeekToFirst() override { itr_ = table_.begin(); }

  void SeekToLast() override {
    itr_ = table_.end();
    --itr_;
  }

  void Seek(const Slice& target) override {
    std::string str_target(target.data(), target.size());
    itr_ = table_.lower_bound(str_target);
  }

  void Next() override { ++itr_; }

  void Prev() override {
    if (itr_ == table_.begin()) {
      itr_ = table_.end();
    } else {
      --itr_;
    }
  }

  Slice key() const override { return Slice(itr_->first); }

  Slice value() const override { return Slice(itr_->second); }

  Status status() const override { return Status::OK(); }

 private:
  const MockFileContents& table_;
  MockFileContents::const_iterator itr_;
};

class MockTableBuilder : public TableBuilder {
 public:
  MockTableBuilder(uint32_t id, MockTableFileSystem* file_system,
                   CompressionType compression_type)
      : id_(id),
        file_system_(file_system),
        compression_type_(compression_type) {}

  // REQUIRES: Either Finish() or Abandon() has been called.
  ~MockTableBuilder() {}

  // Add key,value to the table being constructed.
  // REQUIRES: key is after any previously added key according to comparator.
  // REQUIRES: Finish(), Abandon() have not been called
  void Add(const Slice& key, const Slice& value) override {
    table_.insert({key.ToString(), value.ToString()});
  }

  // Return non-ok iff some error has been detected.
  Status status() const override { return Status::OK(); }

  Status Finish() override {
    if (finish_cb_ != nullptr) {
      (*finish_cb_)(compression_type_, FileSize());
    }
    MutexLock lock_guard(&file_system_->mutex);
    file_system_->files.insert({id_, table_});
    return Status::OK();
  }

  void Abandon() override {}

  uint64_t NumEntries() const override { return table_.size(); }

  uint64_t FileSize() const override { return table_.size(); }

  static std::function<void(const CompressionType&, uint64_t)>* finish_cb_;

 private:
  uint32_t id_;
  MockTableFileSystem* file_system_;
  MockFileContents table_;
  CompressionType compression_type_;
};

class MockTableFactory : public TableFactory {
 public:
  MockTableFactory();
  const char* Name() const override { return "MockTable"; }
  Status NewTableReader(const ImmutableCFOptions& ioptions,
                               const EnvOptions& env_options,
                               const InternalKeyComparator& internal_key,
                               unique_ptr<RandomAccessFile>&& file, uint64_t file_size,
                               unique_ptr<TableReader>* table_reader) const override;
  TableBuilder* NewTableBuilder(const ImmutableCFOptions& ioptions,
                                const InternalKeyComparator& internal_key,
                                WritableFile* file,
                                const CompressionType compression_type,
                                const CompressionOptions& compression_opts,
                                const bool skip_filters = false) const override;

  // This function will directly create mock table instead of going through
  // MockTableBuilder. MockFileContents has to have a format of <internal_key,
  // value>. Those key-value pairs will then be inserted into the mock table
  Status CreateMockTable(Env* env, const std::string& fname,
                         MockFileContents file_contents);

  virtual Status SanitizeOptions(
      const DBOptions& db_opts,
      const ColumnFamilyOptions& cf_opts) const override {
    return Status::OK();
  }

  virtual std::string GetPrintableTableOptions() const override {
    return std::string();
  }

  // This function will assert that only a single file exists and that the
  // contents are equal to file_contents
  void AssertSingleFile(const MockFileContents& file_contents);
  void AssertLatestFile(const MockFileContents& file_contents);

 private:
  uint32_t GetAndWriteNextID(WritableFile* file) const;
  uint32_t GetIDFromFile(RandomAccessFile* file) const;

  mutable MockTableFileSystem file_system_;
  mutable std::atomic<uint32_t> next_id_;
};

}  // namespace mock
}  // namespace rocksdb
