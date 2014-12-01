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

#include "../stream.h"
#include "../utf8.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s out.utf8\n\n"
            "Generates all legal UTF-8 codepoints from 0 to 65,536, excluding\n"
            "the surrogate pair range [0xD800, 0xDFFF].\n",
            argv[0]);
    return -1;
  }
  FILE* fp = fopen(argv[1], "wb");
  webgl_loader::FileSink sink(fp);
  for (size_t word = 0; word < 65536; ++word) {
    webgl_loader::Uint16ToUtf8(word, &sink);
  }
  fclose(fp);
  return 0;
}
