//  Copyright (c) 2013, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.

#include <map>
#include <string>

#include "thirdparty/rocksdb-3.10/db/compaction_job.h"
#include "thirdparty/rocksdb-3.10/db/column_family.h"
#include "thirdparty/rocksdb-3.10/db/version_set.h"
#include "thirdparty/rocksdb-3.10/db/writebuffer.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/cache.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/options.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/db.h"
#include "thirdparty/rocksdb-3.10/util/string_util.h"
#include "thirdparty/rocksdb-3.10/util/testharness.h"
#include "thirdparty/rocksdb-3.10/util/testutil.h"
#include "thirdparty/rocksdb-3.10/table/mock_table.h"

namespace rocksdb {

// TODO(icanadi) Make it simpler once we mock out VersionSet
class CompactionJobTest : public testing::Test {
 public:
  CompactionJobTest()
      : env_(Env::Default()),
        dbname_(test::TmpDir() + "/compaction_job_test"),
        mutable_cf_options_(Options(), ImmutableCFOptions(Options())),
        table_cache_(NewLRUCache(50000, 16)),
        write_buffer_(db_options_.db_write_buffer_size),
        versions_(new VersionSet(dbname_, &db_options_, env_options_,
                                 table_cache_.get(), &write_buffer_,
                                 &write_controller_)),
        shutting_down_(false),
        mock_table_factory_(new mock::MockTableFactory()) {
    EXPECT_OK(env_->CreateDirIfMissing(dbname_));
    db_options_.db_paths.emplace_back(dbname_,
                                      std::numeric_limits<uint64_t>::max());
    NewDB();
    std::vector<ColumnFamilyDescriptor> column_families;
    cf_options_.table_factory = mock_table_factory_;
    column_families.emplace_back(kDefaultColumnFamilyName, cf_options_);

    EXPECT_OK(versions_->Recover(column_families, false));
  }

  std::string GenerateFileName(uint64_t file_number) {
    FileMetaData meta;
    std::vector<DbPath> db_paths;
    db_paths.emplace_back(dbname_, std::numeric_limits<uint64_t>::max());
    meta.fd = FileDescriptor(file_number, 0, 0);
    return TableFileName(db_paths, meta.fd.GetNumber(), meta.fd.GetPathId());
  }

  // returns expected result after compaction
  mock::MockFileContents CreateTwoFiles() {
    mock::MockFileContents expected_results;
    const int kKeysPerFile = 10000;
    SequenceNumber sequence_number = 0;
    for (int i = 0; i < 2; ++i) {
      mock::MockFileContents contents;
      SequenceNumber smallest_seqno = 0, largest_seqno = 0;
      InternalKey smallest, largest;
      for (int k = 0; k < kKeysPerFile; ++k) {
        auto key = ToString(i * (kKeysPerFile / 2) + k);
        auto value = ToString(i * kKeysPerFile + k);
        InternalKey internal_key(key, ++sequence_number, kTypeValue);
        if (k == 0) {
          smallest = internal_key;
          smallest_seqno = sequence_number;
        } else if (k == kKeysPerFile - 1) {
          largest = internal_key;
          largest_seqno = sequence_number;
        }
        std::pair<std::string, std::string> key_value(
            {internal_key.Encode().ToString(), value});
        contents.insert(key_value);
        if (i == 1 || k < kKeysPerFile / 2) {
          expected_results.insert(key_value);
        }
      }

      uint64_t file_number = versions_->NewFileNumber();
      EXPECT_OK(mock_table_factory_->CreateMockTable(
          env_, GenerateFileName(file_number), std::move(contents)));

      VersionEdit edit;
      edit.AddFile(0, file_number, 0, 10, smallest, largest, smallest_seqno,
                   largest_seqno);

      mutex_.Lock();
      versions_->LogAndApply(versions_->GetColumnFamilySet()->GetDefault(),
                             mutable_cf_options_, &edit, &mutex_);
      mutex_.Unlock();
    }
    versions_->SetLastSequence(sequence_number);
    return expected_results;
  }

  void NewDB() {
    VersionEdit new_db;
    new_db.SetLogNumber(0);
    new_db.SetNextFile(2);
    new_db.SetLastSequence(0);

    const std::string manifest = DescriptorFileName(dbname_, 1);
    unique_ptr<WritableFile> file;
    Status s = env_->NewWritableFile(
        manifest, &file, env_->OptimizeForManifestWrite(env_options_));
    ASSERT_OK(s);
    {
      log::Writer log(std::move(file));
      std::string record;
      new_db.EncodeTo(&record);
      s = log.AddRecord(record);
    }
    ASSERT_OK(s);
    // Make "CURRENT" file that points to the new manifest file.
    s = SetCurrentFile(env_, dbname_, 1, nullptr);
  }

  Env* env_;
  std::string dbname_;
  EnvOptions env_options_;
  MutableCFOptions mutable_cf_options_;
  std::shared_ptr<Cache> table_cache_;
  WriteController write_controller_;
  DBOptions db_options_;
  ColumnFamilyOptions cf_options_;
  WriteBuffer write_buffer_;
  std::unique_ptr<VersionSet> versions_;
  InstrumentedMutex mutex_;
  std::atomic<bool> shutting_down_;
  std::shared_ptr<mock::MockTableFactory> mock_table_factory_;
};

TEST_F(CompactionJobTest, Simple) {
  auto cfd = versions_->GetColumnFamilySet()->GetDefault();

  auto expected_results = CreateTwoFiles();

  auto files = cfd->current()->storage_info()->LevelFiles(0);
  ASSERT_EQ(2U, files.size());

  std::unique_ptr<Compaction> compaction(Compaction::TEST_NewCompaction(
      7, 0, 1, 1024 * 1024, 10, 0, kNoCompression));
  compaction->SetInputVersion(cfd->current());

  auto compaction_input_files = compaction->TEST_GetInputFiles(0);
  compaction_input_files->level = 0;
  compaction_input_files->files.push_back(files[0]);
  compaction_input_files->files.push_back(files[1]);

  SnapshotList snapshots;
  int yield_callback_called = 0;
  std::function<uint64_t()> yield_callback = [&]() {
    yield_callback_called++;
    return 0;
  };
  LogBuffer log_buffer(InfoLogLevel::INFO_LEVEL, db_options_.info_log.get());
  mutex_.Lock();
  CompactionJob compaction_job(0, compaction.get(), db_options_,
                               *cfd->GetLatestMutableCFOptions(), env_options_,
                               versions_.get(), &shutting_down_, &log_buffer,
                               nullptr, nullptr, nullptr, &snapshots, true,
                               table_cache_, std::move(yield_callback));
  compaction_job.Prepare();
  mutex_.Unlock();
  ASSERT_OK(compaction_job.Run());
  mutex_.Lock();
  Status s;
  compaction_job.Install(&s, &mutex_);
  ASSERT_OK(s);
  mutex_.Unlock();

  mock_table_factory_->AssertLatestFile(expected_results);
  ASSERT_EQ(yield_callback_called, 20000);
}

}  // namespace rocksdb

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
