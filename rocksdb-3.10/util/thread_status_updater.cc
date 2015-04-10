// Copyright (c) 2014, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#include <memory>
#include "thirdparty/rocksdb-3.10/include/rocksdb/env.h"
#include "thirdparty/rocksdb-3.10/port/likely.h"
#include "thirdparty/rocksdb-3.10/util/mutexlock.h"
#include "thirdparty/rocksdb-3.10/util/thread_status_updater.h"

namespace rocksdb {

#if ROCKSDB_USING_THREAD_STATUS

__thread ThreadStatusData* ThreadStatusUpdater::thread_status_data_ = nullptr;

void ThreadStatusUpdater::UnregisterThread() {
  if (thread_status_data_ != nullptr) {
    std::lock_guard<std::mutex> lck(thread_list_mutex_);
    thread_data_set_.erase(thread_status_data_);
    delete thread_status_data_;
    thread_status_data_ = nullptr;
  }
}

void ThreadStatusUpdater::SetThreadType(
    ThreadStatus::ThreadType ttype) {
  auto* data = InitAndGet();
  data->thread_type.store(ttype, std::memory_order_relaxed);
}

void ThreadStatusUpdater::ResetThreadStatus() {
  ClearThreadState();
  ClearThreadOperation();
  SetColumnFamilyInfoKey(nullptr);
}

void ThreadStatusUpdater::SetColumnFamilyInfoKey(
    const void* cf_key) {
  auto* data = InitAndGet();
  // set the tracking flag based on whether cf_key is non-null or not.
  // If enable_thread_tracking is set to false, the input cf_key
  // would be nullptr.
  data->enable_tracking = (cf_key != nullptr);
  data->cf_key.store(cf_key, std::memory_order_relaxed);
}

const void* ThreadStatusUpdater::GetColumnFamilyInfoKey() {
  auto* data = InitAndGet();
  if (data->enable_tracking == false) {
    return nullptr;
  }
  return data->cf_key.load(std::memory_order_relaxed);
}

void ThreadStatusUpdater::SetThreadOperation(
    const ThreadStatus::OperationType type) {
  auto* data = InitAndGet();
  if (!data->enable_tracking) {
    assert(data->cf_key.load(std::memory_order_relaxed) == nullptr);
    return;
  }
  data->operation_stage.store(ThreadStatus::STAGE_UNKNOWN,
      std::memory_order_relaxed);
  data->operation_type.store(type, std::memory_order_relaxed);
}

void ThreadStatusUpdater::SetOperationStartTime(const uint64_t start_time) {
  auto* data = InitAndGet();
  if (!data->enable_tracking) {
    assert(data->cf_key.load(std::memory_order_relaxed) == nullptr);
    return;
  }
  data->op_start_time.store(start_time, std::memory_order_relaxed);
}

void ThreadStatusUpdater::ClearThreadOperation() {
  auto* data = InitAndGet();
  if (!data->enable_tracking) {
    assert(data->cf_key.load(std::memory_order_relaxed) == nullptr);
    return;
  }
  data->operation_stage.store(ThreadStatus::STAGE_UNKNOWN,
      std::memory_order_relaxed);
  data->operation_type.store(
      ThreadStatus::OP_UNKNOWN, std::memory_order_relaxed);
}

ThreadStatus::OperationStage ThreadStatusUpdater::SetThreadOperationStage(
    ThreadStatus::OperationStage stage) {
  auto* data = InitAndGet();
  if (!data->enable_tracking) {
    assert(data->cf_key.load(std::memory_order_relaxed) == nullptr);
    return ThreadStatus::STAGE_UNKNOWN;
  }
  return data->operation_stage.exchange(
      stage, std::memory_order_relaxed);
}

void ThreadStatusUpdater::SetThreadState(
    const ThreadStatus::StateType type) {
  auto* data = InitAndGet();
  if (!data->enable_tracking) {
    assert(data->cf_key.load(std::memory_order_relaxed) == nullptr);
    return;
  }
  data->state_type.store(type, std::memory_order_relaxed);
}

void ThreadStatusUpdater::ClearThreadState() {
  auto* data = InitAndGet();
  if (!data->enable_tracking) {
    assert(data->cf_key.load(std::memory_order_relaxed) == nullptr);
    return;
  }
  data->state_type.store(
      ThreadStatus::STATE_UNKNOWN, std::memory_order_relaxed);
}

Status ThreadStatusUpdater::GetThreadList(
    std::vector<ThreadStatus>* thread_list) {
  thread_list->clear();
  std::vector<std::shared_ptr<ThreadStatusData>> valid_list;
  uint64_t now_micros = Env::Default()->NowMicros();

  std::lock_guard<std::mutex> lck(thread_list_mutex_);
  for (auto* thread_data : thread_data_set_) {
    assert(thread_data);
    auto thread_type = thread_data->thread_type.load(
        std::memory_order_relaxed);
    // Since any change to cf_info_map requires thread_list_mutex,
    // which is currently held by GetThreadList(), here we can safely
    // use "memory_order_relaxed" to load the cf_key.
    auto cf_key = thread_data->cf_key.load(
        std::memory_order_relaxed);
    auto iter = cf_info_map_.find(cf_key);
    assert(cf_key == 0 || iter != cf_info_map_.end());
    auto* cf_info = iter != cf_info_map_.end() ?
        iter->second.get() : nullptr;
    const std::string* db_name = nullptr;
    const std::string* cf_name = nullptr;
    ThreadStatus::OperationType op_type = ThreadStatus::OP_UNKNOWN;
    ThreadStatus::OperationStage op_stage = ThreadStatus::STAGE_UNKNOWN;
    ThreadStatus::StateType state_type = ThreadStatus::STATE_UNKNOWN;
    uint64_t op_elapsed_micros = 0;
    if (cf_info != nullptr) {
      db_name = &cf_info->db_name;
      cf_name = &cf_info->cf_name;
      op_type = thread_data->operation_type.load(
          std::memory_order_relaxed);
      // display lower-level info only when higher-level info is available.
      if (op_type != ThreadStatus::OP_UNKNOWN) {
        op_elapsed_micros = now_micros - thread_data->op_start_time.load(
            std::memory_order_relaxed);
        op_stage = thread_data->operation_stage.load(
            std::memory_order_relaxed);
        state_type = thread_data->state_type.load(
            std::memory_order_relaxed);
      }
    }
    thread_list->emplace_back(
        thread_data->thread_id, thread_type,
        db_name ? *db_name : "",
        cf_name ? *cf_name : "",
        op_type, op_elapsed_micros, op_stage, state_type);
  }

  return Status::OK();
}

ThreadStatusData* ThreadStatusUpdater::InitAndGet() {
  if (UNLIKELY(thread_status_data_ == nullptr)) {
    thread_status_data_ = new ThreadStatusData();
    thread_status_data_->thread_id = reinterpret_cast<uint64_t>(
        thread_status_data_);
    std::lock_guard<std::mutex> lck(thread_list_mutex_);
    thread_data_set_.insert(thread_status_data_);
  }
  return thread_status_data_;
}

void ThreadStatusUpdater::NewColumnFamilyInfo(
    const void* db_key, const std::string& db_name,
    const void* cf_key, const std::string& cf_name) {
  // Acquiring same lock as GetThreadList() to guarantee
  // a consistent view of global column family table (cf_info_map).
  std::lock_guard<std::mutex> lck(thread_list_mutex_);

  cf_info_map_[cf_key].reset(
      new ConstantColumnFamilyInfo(db_key, db_name, cf_name));
  db_key_map_[db_key].insert(cf_key);
}

void ThreadStatusUpdater::EraseColumnFamilyInfo(const void* cf_key) {
  // Acquiring same lock as GetThreadList() to guarantee
  // a consistent view of global column family table (cf_info_map).
  std::lock_guard<std::mutex> lck(thread_list_mutex_);
  auto cf_pair = cf_info_map_.find(cf_key);
  assert(cf_pair != cf_info_map_.end());

  auto* cf_info = cf_pair->second.get();
  assert(cf_info);

  // Remove its entry from db_key_map_ by the following steps:
  // 1. Obtain the entry in db_key_map_ whose set contains cf_key
  // 2. Remove it from the set.
  auto db_pair = db_key_map_.find(cf_info->db_key);
  assert(db_pair != db_key_map_.end());
  size_t result __attribute__((unused)) = db_pair->second.erase(cf_key);
  assert(result);

  cf_pair->second.reset();
  result = cf_info_map_.erase(cf_key);
  assert(result);
}

void ThreadStatusUpdater::EraseDatabaseInfo(const void* db_key) {
  // Acquiring same lock as GetThreadList() to guarantee
  // a consistent view of global column family table (cf_info_map).
  std::lock_guard<std::mutex> lck(thread_list_mutex_);
  auto db_pair = db_key_map_.find(db_key);
  if (UNLIKELY(db_pair == db_key_map_.end())) {
    // In some occasional cases such as DB::Open fails, we won't
    // register ColumnFamilyInfo for a db.
    return;
  }

  size_t result __attribute__((unused)) = 0;
  for (auto cf_key : db_pair->second) {
    auto cf_pair = cf_info_map_.find(cf_key);
    assert(cf_pair != cf_info_map_.end());
    cf_pair->second.reset();
    result = cf_info_map_.erase(cf_key);
    assert(result);
  }
  db_key_map_.erase(db_key);
}

#else

void ThreadStatusUpdater::UnregisterThread() {
}

void ThreadStatusUpdater::ResetThreadStatus() {
}

void ThreadStatusUpdater::SetThreadType(
    ThreadStatus::ThreadType ttype) {
}

void ThreadStatusUpdater::SetColumnFamilyInfoKey(
    const void* cf_key) {
}

void ThreadStatusUpdater::SetThreadOperation(
    const ThreadStatus::OperationType type) {
}

void ThreadStatusUpdater::ClearThreadOperation() {
}

void ThreadStatusUpdater::SetThreadState(
    const ThreadStatus::StateType type) {
}

void ThreadStatusUpdater::ClearThreadState() {
}

Status ThreadStatusUpdater::GetThreadList(
    std::vector<ThreadStatus>* thread_list) {
  return Status::NotSupported(
      "GetThreadList is not supported in the current running environment.");
}

void ThreadStatusUpdater::NewColumnFamilyInfo(
    const void* db_key, const std::string& db_name,
    const void* cf_key, const std::string& cf_name) {
}

void ThreadStatusUpdater::EraseColumnFamilyInfo(const void* cf_key) {
}

void ThreadStatusUpdater::EraseDatabaseInfo(const void* db_key) {
}

#endif  // ROCKSDB_USING_THREAD_STATUS
}  // namespace rocksdb
