// Copyright 2012 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You
// may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language governing
// permissions and limitations under the License.

#ifndef WEBGL_LOADER_JSON_H_
#define WEBGL_LOADER_JSON_H_

#include <float.h>
#include <stdio.h>
#include <string.h>

#include <vector>

#include "base.h"
#include "stream.h"

namespace webgl_loader {

// JsonSink will generate JSON in the ByteSink passed in, but does
// not actually own the backing data. Performs rudimentary grammar
// checking. It will automatically add delimiting punctuation and
// prevent non-String object keys.
//
// TODO: Pretty printing.
class JsonSink {
 public:
  // |sink| is unowned and should not be NULL.
  explicit JsonSink(ByteSinkInterface* sink)
    : sink_(sink) {
    state_.reserve(8);
    PushState(JSON_STATE_SIMPLE);
  }

  // Automatically close values when JsonSink goes out of scope.
  ~JsonSink() {
    EndAll();
  }

  // Methods to put scalar values into the JSON object. Aside from
  // PutString, these should not be called when JsonSink expects an
  // object key.
  void PutNull() {
    OnPutValue();
    PutN("null", 4);
  }

  void PutBool(bool b) {
    OnPutValue();
    PutN(b ? "true" : "false", b ? 4 : 5);
  }

  void PutInt(int i) {
    OnPutValue();
    char buf[kBufSize];
    int len = sprintf(buf, "%d", i);
    CHECK(len > 0 && len < kBufSize);
    PutN(buf, len);
  }

  void PutFloat(float f) {
    OnPutValue();
    char buf[kBufSize];
    int len = sprintf(buf, "%g", f);
    CHECK(len > 0 && len < kBufSize);
    PutN(buf, len);
  }

  // |str| should not be NULL.
  void PutString(const char* str) {
    // Strings are the only legal value for object keys.
    switch (GetState()) {
    case JSON_STATE_OBJECT_KEY:
      Put(',');  // fall through.
    case JSON_STATE_OBJECT_KEY_FIRST:
      SetState(JSON_STATE_OBJECT_VALUE);
      break;
    default:
      UpdateState();
    }

    // TODO: escaping.
    Put('\"');
    PutN(str, strlen(str));
    Put('\"');
  }

  // Arrays and Objects are recursive JSON values.
  void BeginArray() {
    OnPutValue();
    Put('[');
    PushState(JSON_STATE_ARRAY_FIRST);
  }

  void BeginObject() {
    OnPutValue();
    Put('{');
    PushState(JSON_STATE_OBJECT_KEY_FIRST);
  }

  // Close recurisve value, if necessary. Will automatically pad null
  // values to unmatched object keys, if necessary.
  void End() {
    switch (GetState()) {
    case JSON_STATE_OBJECT_VALUE:
      // We haven't provided a value, so emit a null..
      PutNull();  // ...and fall through to the normal case.
    case JSON_STATE_OBJECT_KEY:
    case JSON_STATE_OBJECT_KEY_FIRST:
      Put('}');
      break;
    case JSON_STATE_ARRAY_FIRST:
    case JSON_STATE_ARRAY:
      Put(']');
      break;
    default: 
      return;  // Do nothing.
    }
    PopState();
  }
  
  // Close all values. Convenient way to end up with a valid JSON
  // object.
  void EndAll() {
    while (GetState() != JSON_STATE_SIMPLE) End();
  }
  
 private:
  // Conservative estimate of how many bytes to allocate on the stack
  // as scratch space for int/float to NUL-terminated string.
  static const int kBufSize = 32;
  
  // JsonSink needs to internally maintain some structural state in
  // order to correctly delimit values with the appropriate
  // punctuation. |State| indicates what the desired next value should
  // be.
  enum State {
    JSON_STATE_OBJECT_VALUE,
    JSON_STATE_OBJECT_KEY_FIRST,
    JSON_STATE_OBJECT_KEY,
    JSON_STATE_ARRAY_FIRST,
    JSON_STATE_ARRAY,
    JSON_STATE_SIMPLE
  };

  // State stack helpers. Because JSON allows for recurisve values,
  // maintain a stack of State enums.
  State GetState() const {
    return state_.back();
  }

  void SetState(State state) {
    state_.back() = state;
  }

  void PushState(State state) {
    state_.push_back(state);
  }

  void PopState() {
    state_.pop_back();
  }
  
  // State transducer.
  void UpdateState() {
    switch (GetState()) {
    case JSON_STATE_OBJECT_VALUE:
      Put(':');
      SetState(JSON_STATE_OBJECT_KEY);
      return;
    case JSON_STATE_ARRAY_FIRST:
      SetState(JSON_STATE_ARRAY);
      return;
    case JSON_STATE_ARRAY:
      Put(',');
      return;
    default:
      return;
    }
  }

  void CheckNotKey() const {
    CHECK(GetState() != JSON_STATE_OBJECT_KEY_FIRST ||
	  GetState() != JSON_STATE_OBJECT_KEY);
  }

  // A helper method for scalars (besides strings) that prevents them
  // from being used as object keys.
  void OnPutValue() {
    CheckNotKey();
    UpdateState();
  }

  // Convenience forwarding methods for sink_.
  void Put(char c) {
    sink_->Put(c);
  }
  
  void PutN(const char* str, size_t len) {
    sink_->PutN(str, len);
  }
  
  ByteSinkInterface* sink_;
  std::vector<State> state_;
};

}  // namespace webgl_loader

#endif  // WEBGL_LOADER_JSON_H_
