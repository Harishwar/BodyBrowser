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

#include "base.h"
#include "stream.h"

namespace webgl_loader {

void PrintByteHistogram(FILE* fp) {
  const size_t kBufSize = 8192;
  char buf[kBufSize];

  BufferedInputStream input(fp, buf, kBufSize);
  NullSink null_sink;
  ByteHistogramSink histo_sink(&null_sink);

  while (kNoError == input.Refill()) {
    const size_t count = input.end() - input.cursor;
    histo_sink.PutN(input.cursor, count);
    input.cursor += count;
  }

  const size_t* histo = histo_sink.histo();
  size_t max = 0;
  for (size_t i = 0; i < 256; ++i) {
    if (histo[i] > max) max = histo[i];
  }

  const char lollipop[] = 
      "---------------------------------------------------------------------o";
  const double scale = sizeof(lollipop) / double(max);

  for (int i = 0; i < 256; ++i) {
    const int width = scale * histo[i];
    printf("%3d: %s\n", i, lollipop + (sizeof(lollipop) - width));
  }
}

}  // namespace webgl_loader

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [in]\n\n"
            "\tReads binary file |in| and prints byte historgram to STDOUT."
            "\n\n",
            argv[0]);
    return -1;
  }
  FILE* fp = fopen(argv[1], "rb");
  if (!fp) {
    fprintf(stderr, "Could not open file: %s\n\n", argv[1]); 
    return -1;
  }
  webgl_loader::PrintByteHistogram(fp);
  fclose(fp);
  return 0;
}
