/**
 * A MergeOperator for rocksdb that implements string append.
 * @author Deon Nicholas (dnicholas@fb.com)
 * Copyright 2013 Facebook
 */

#pragma once
#include "thirdparty/rocksdb-3.10/include/rocksdb/merge_operator.h"
#include "thirdparty/rocksdb-3.10/include/rocksdb/slice.h"

namespace rocksdb {

class StringAppendOperator : public AssociativeMergeOperator {
 public:
  StringAppendOperator(char delim_char);    /// Constructor: specify delimiter

  virtual bool Merge(const Slice& key,
                     const Slice* existing_value,
                     const Slice& value,
                     std::string* new_value,
                     Logger* logger) const override;

  virtual const char* Name() const override;

 private:
  char delim_;         // The delimiter is inserted between elements

};

} // namespace rocksdb

