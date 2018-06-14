// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include <iostream>
#include "llvm/Support/raw_ostream.h"
void disableStreamsBuffering() {
#ifdef _WIN32
  // Avoid debugger slow down.
  // http://ofekshilon.com/2014/09/20/accelerating-debug-runs-part-1-_no_debug_heap-2/
  _putenv_s("_NO_DEBUG_HEAP", "1");
#endif
  std::ios::sync_with_stdio();
  // No stream buffering.
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::cin.clear();
  std::cout.clear();
  std::cerr.clear();
  llvm::outs().SetUnbuffered();
}
