// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "ErrorCouldNot.h"
#include "llvm/Support/Compiler.h"
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>

#ifdef LLVM_ON_WIN32
#include <windows.h>

void ErrorCouldNot(const char *FileName, const char *What) {
  char Msg[1024];
  _snprintf(Msg, sizeof(Msg), "Ceemple: error %u could not %s\n\n%s",
            GetLastError(), What, FileName);
  MessageBoxA(NULL, Msg, NULL, MB_OK);
}
#else // LLVM_ON_WIN32

#include <errno.h>
void ErrorCouldNot(const char *FileName, const char *What) {
  char Msg[1024];
  snprintf(Msg, sizeof(Msg), "Ceemple: error %u could not %s\n\n%s", errno,
           What, FileName);
}
#endif // LLVM_ON_WIN32
