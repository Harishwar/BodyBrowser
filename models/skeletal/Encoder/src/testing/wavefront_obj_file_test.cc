#if 0  // A cute trick to making this .cc self-building from shell.
g++ $0 -Wall -Werror -o `basename $0 .cc`;
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

#include <string.h>

#define private public

#include "../base.h"
#include "../mesh.h"

#define CHECK_INDICES(POSITION_INDEX, TEXCOORD_INDEX, NORMAL_INDEX) \
  CHECK(POSITION_INDEX == position_index_);                         \
  CHECK(TEXCOORD_INDEX == texcoord_index_);                         \
  CHECK(NORMAL_INDEX == normal_index_)

#define PARSE_INDICES(LINE) \
  CHECK(LINE + strlen(LINE) == ParseIndices(LINE))

class ParseIndicesTester {
 public:
  ParseIndicesTester()
      : position_index_(0),
        texcoord_index_(0),
        normal_index_(0) {
  }
  
  void Test() {
    CHECK(NULL == ParseIndices(""));
    CHECK(NULL == ParseIndices("nodigit"));
    const char kBasic[] = "1/2/3";
    PARSE_INDICES(kBasic);
    CHECK_INDICES(1, 2, 3);
    const char kNoTexCoord[] = "4//5";
    PARSE_INDICES(kNoTexCoord);
    CHECK_INDICES(4, 0, 5);
    const char kUno[] = "6";
    PARSE_INDICES(kUno);
    CHECK_INDICES(6, 0, 0);
    const char kBad[] = "7/?/8";
    ParseIndices(kBad);
    CHECK_INDICES(7, 0, 0);
    const char kThree[] = " 1/122/1 2/123/2 3/117/3";
    const char* next = ParseIndices(kThree);
    CHECK_INDICES(1, 122, 1);
    next = ParseIndices(next);
    CHECK_INDICES(2, 123, 2);
    CHECK(kThree + strlen(kThree) == ParseIndices(next));
    CHECK_INDICES(3, 117, 3);
  }
  
 private:
  const char* ParseIndices(const char* line) {
    return obj_.ParseIndices(line, 0, &position_index_, &texcoord_index_,
                             &normal_index_);
  }

  int position_index_;
  int texcoord_index_;
  int normal_index_;
  WavefrontObjFile obj_;
};

int main(int argc, char* argv[]) {
  ParseIndicesTester tester;
  tester.Test();
  return 0;
}
