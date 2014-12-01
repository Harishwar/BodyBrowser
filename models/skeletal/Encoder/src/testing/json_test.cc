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

#include "../base.h"
#include "../stream.h"
#include "../json.h"

#include <float.h>
#include <limits.h>
#include <string>

namespace webgl_loader {

class JsonSinkTest {
 public:
  JsonSinkTest()
    : sink_(&buf_), json_(&sink_) {
  }

  void TestNull() {
    json_.PutNull();
    CheckString("null");
  }

  void TestBool() {
    json_.PutBool(true);
    CheckString("true");
    json_.PutBool(false);
    CheckString("false");
  }

  void TestInt() {
    for (int i = 0; i < 10; ++i) {
      json_.PutInt(i);
      char test[] = "0";
      test[0] += i;
      CheckString(test);
    }
    json_.PutInt(INT_MIN);
    CheckString("-2147483648");
    json_.PutInt(INT_MAX);
    CheckString("2147483647");
  }

  void TestFloat() {
    json_.PutFloat(123.456);
    CheckString("123.456");
    json_.PutFloat(FLT_MAX);
    CheckString("3.40282e+38");
    json_.PutFloat(-FLT_MAX);
    CheckString("-3.40282e+38");
    json_.PutFloat(FLT_MIN);
    CheckString("1.17549e-38");
    json_.PutFloat(-FLT_MIN);
    CheckString("-1.17549e-38");
  }

  void TestString() {
    json_.PutString("foo");
    CheckString("\"foo\"");
  }

  void TestArray() {
    json_.BeginArray();
    for (int i = 0; i < 100; i += 10) {
      json_.PutInt(i);
    }
    json_.End();
    CheckString("[0,10,20,30,40,50,60,70,80,90]");

    json_.BeginArray();
    json_.BeginArray();
    json_.PutNull();
    json_.End();
    json_.BeginArray();
    json_.PutBool(false);
    json_.PutBool(true);
    json_.End();
    for (int i = 0; i < 5; ++i) {
      json_.PutInt(i*i);
    }
    json_.BeginObject();
    json_.PutString("key");
    json_.PutString("value");
    json_.EndAll();

    CheckString("[[null],[false,true],0,1,4,9,16,{\"key\":\"value\"}]");
  }

  void TestObject() {
    json_.BeginObject();
    json_.PutString("key1");
    json_.PutInt(1);
    json_.PutString("keyABC");
    json_.PutString("abc");
    json_.End();
    
    CheckString("{\"key1\":1,\"keyABC\":\"abc\"}");

    json_.BeginObject();
    json_.PutString("array");
    json_.BeginArray();
    for (int i = 1; i <= 3; ++i) {
      json_.PutInt(i);
    }
    json_.End();
    json_.BeginObject();
    json_.PutString("key");
    json_.PutString("value");
    json_.End();
    json_.PutString("k");
    json_.PutFloat(0.1);
    json_.End();

    CheckString("{\"array\":[1,2,3]{\"key\":\"value\"},\"k\":0.1}");
  }

 private:
  void CheckString(const char* str) {
    CHECK(buf_ == str);
    buf_.clear();
  }

  std::string buf_;
  StringSink sink_;
  JsonSink json_;
};

}  // namespace webgl_loader

int main() {
  webgl_loader::JsonSinkTest tester;
  tester.TestNull();
  tester.TestBool();
  tester.TestInt();
  tester.TestFloat();
  tester.TestString();
  tester.TestArray();
  tester.TestObject();
    
  return 0;
}
