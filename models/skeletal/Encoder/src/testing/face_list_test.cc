#if 0  // A cute trick to making this .cc self-building from shell.
g++ $0 -O2 -Wall -Werror -o `basename $0 .cc`;
exit;
#endif
// Copyright 2011 Google Inc. All Rights Reserved.
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

#include <stdio.h>

#include <algorithm>

#include "../base.h"

class FaceList {
 public:
  static const size_t kInlinedCapacity = 6;
  
  FaceList()
      : faces_(capacity_or_buffer_.buffer),
        size_(0) {
  }

  FaceList(const FaceList& that)
      : faces_(capacity_or_buffer_.buffer),
        size_(that.size_)
  {
    if (!IsInlined()) {
      faces_ = Malloc(size_);
    }
    Memcpy(faces_, that.faces_, size_);
  }

  ~FaceList() {
    clear();
  }

  size_t size() const {
    return size_;
  }

  size_t capacity() const {
    return (size_ <= kInlinedCapacity) ?
        kInlinedCapacity : capacity_or_buffer_.capacity;
  }

  int& operator[](size_t a) {
    return faces_[a];
  }

  int operator[](size_t a) const {
    return faces_[a];
  }

  typedef int* iterator;
  typedef const int* const_iterator;

  iterator begin() { return faces_; }
  const_iterator begin() const { return faces_; }

  iterator end() { return faces_ + size_; }
  const_iterator end() const { return faces_ + size_; }

  void clear() {
    if (!IsInlined()) {
      free(faces_);
      faces_ = capacity_or_buffer_.buffer;
    }
    size_ = 0;
  }

  int& back() {
    return faces_[size_ - 1];
  }

  int back() const {
    return faces_[size_ - 1];
  }

  void push_back(int i) {
    if (size_ == kInlinedCapacity) {
      const size_t kMinAlloc = 16;
      faces_ = Malloc(kMinAlloc);
      Memcpy(faces_, capacity_or_buffer_.buffer, size_);
      capacity_or_buffer_.capacity = kMinAlloc;
    } else if (!IsInlined() && (size_ == capacity_or_buffer_.capacity)) {
      faces_ = Realloc(faces_, 2*capacity_or_buffer_.capacity);
    }
    faces_[size_++] = i;
  }

  void pop_back() {
    --size_;
    if (size_ == kInlinedCapacity) {
      Memcpy(capacity_or_buffer_.buffer, faces_, kInlinedCapacity);
      free(faces_);
      faces_ = capacity_or_buffer_.buffer;
    }
  }

 private:
  bool IsInlined() const {
    return size_ <= kInlinedCapacity;
  }

  static void Memcpy(int *to, const int* from, size_t n) {
    memcpy(to, from, sizeof(int)*n);
  }

  static int* Malloc(size_t sz) {
    return static_cast<int*>(malloc(sizeof(int)*sz));
  }

  static int* Realloc(int *ptr, size_t sz) {
    return static_cast<int*>(realloc(ptr, sizeof(int)*sz));
  }
  
  int* faces_;
  size_t size_;
  union {
    size_t capacity; 
    int buffer[kInlinedCapacity];
  } capacity_or_buffer_;
};

void TestBasic() {
  FaceList face_list;
  CHECK(0 == face_list.size());
  face_list.push_back(10);
  CHECK(1 == face_list.size());
  CHECK(10 == face_list.back());
  face_list.push_back(20);
  CHECK(2 == face_list.size());
  CHECK(20 == face_list.back());
  face_list.back() = 30;
  CHECK(2 == face_list.size());
  CHECK(30 == face_list.back());
  face_list.clear();
  CHECK(0 == face_list.size());
}

void TestPushClear(size_t n) {
  FaceList face_list;
  std::vector<int> v;
  for (size_t i = 0; i < n; ++i) {
    const int to_push = i * i;
    face_list.push_back(to_push);
    v.push_back(to_push);
    CHECK(v.size() == face_list.size());
    CHECK(to_push == face_list.back());
  }
  CHECK(n == face_list.size());
  CHECK(std::equal(face_list.begin(), face_list.end(), v.begin()));
  FaceList copied(face_list);
  CHECK(n == copied.size());
  CHECK(std::equal(face_list.begin(), face_list.end(), copied.begin()));
  face_list.clear();
  CHECK(0 == face_list.size());
}

void TestPushPopBack(size_t n) {
  FaceList face_list;
  std::vector<int> v;
  for (size_t i = 0; i < n; ++i) {
    const int to_push = i * i;
    face_list.push_back(to_push);
    v.push_back(to_push);
    CHECK(v.size() == face_list.size());
    CHECK(to_push == face_list.back());
  }
  for (size_t i = n - 1; i > 0; --i) {
    const int to_pop = i * i;
    CHECK(v.size() == face_list.size());
    CHECK(to_pop == face_list.back());
    face_list.pop_back();
    v.pop_back();
  }
  CHECK(0 == face_list.back());
  CHECK(1 == face_list.size());
  face_list.pop_back();
  CHECK(0 == face_list.size());
}

int main(int argc, char* argv[]) {
  TestBasic();
  // Chose sizes to guarante both inlined->extern transition and
  // realloc.
  for (size_t i = 4; i < 20; ++i) {
    TestPushClear(i);
    TestPushPopBack(i);
  }
  return 0;
}
