// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "Client.h"

#include "llvm/Support/raw_ostream.h"

#include "clang/Driver/Driver.h"

static bool useClang(llvm::ArrayRef<const char *> Argv) {
  bool Assembly = false;
  bool EmitLLVM = false;
  for (const char *arg : Argv) {
    if (!arg)
      continue;
    llvm::StringRef Arg(arg);
    if (Arg == "-" || Arg == "-emit-pch" || Arg == "-emit-pth" ||
        Arg == "-fmodules" || Arg == "-fdump-record-layouts-simple" ||
        Arg.startswith("-E") || Arg.startswith("-fsanitize") ||
        Arg.startswith("conftest") || Arg.endswith(".c") || Arg.endswith(".m"))
      return true;
    else if (Arg == "-S")
      Assembly = true;
    else if (Arg == "-emit-llvm")
      EmitLLVM = true;
  }
  return Assembly && !EmitLLVM;
}

extern int cc1_main(llvm::ArrayRef<const char *> Argv, const char *Argv0,
                    void *MainAddr);

int cc1_selector(llvm::ArrayRef<const char *> Argv, const char *Argv0,
                 void *MainAddr) {
  if (!useClang(Argv)) {
    int RetCode = startServerAndSend(Argv, Argv0);
    if (RetCode == 0 || RetCode == 1)
      return RetCode;
    assert(RetCode == 2);
  }
  return cc1_main(Argv, Argv0, MainAddr);
}

struct InitZapccClient {
  InitZapccClient() { Zapcc1Main = cc1_selector; }
} InitZapccClient;
