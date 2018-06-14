// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "CachingCompiler.h"
#include "CommandLine.h"
#include "DisableStreamsBuffering.h"
#include "FindOption.h"
#include "GetExePath.h"
#include "Server.h"
#include "StringConst.h"
#include "UnloadSelector.h"
#include "chdir.h"

#include "clang/Frontend/FrontendDiagnostic.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MD5.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <string>

using namespace clang;

static void LLVMErrorHandler(void *UserData, const std::string &Message,
                             bool GenCrashDiag) {
  llvm::errs() << "fatal error: " << Message << '\n';
  llvm::sys::RunSignalHandlers();
  llvm::sys::RunInterruptHandlers();
  exit(GenCrashDiag ? 70 : 1);
}

struct SignalHandlerData {
  Server *Srv;
  CachingCompiler *CC;
};
static void signalHandler(void *data) {
  SignalHandlerData *Data = static_cast<SignalHandlerData *>(data);
  Data->CC->printCommandsTxtName("crash");
}

static bool isIgnoredValueArg(StringRef Arg) {
  // Don't ignore -x since language macros control standard headers.
  StringRef S[] = {"-o",
                   "-D",
                   "-main-file-name",
                   "-I",
                   "-include",
                   "-isystem",
                   "-iquote",
                   "-coverage-file",
                   "-coverage-notes-file",
                   "-internal-isystem",
                   "-internal-externc-isystem",
                   "-dependency-file",
                   "-fdebug-compilation-dir",
                   "-fmessage-length",
                   "-include-pch",
                   "-MT",
                   "-MQ",
                   ServerIdSwitch};
  for (StringRef IgnoredValueArg : S)
    if (Arg == IgnoredValueArg)
      return true;
  return false;
}

static bool isIgnoredArg(StringRef Arg) {
  StringRef IgnoredArgs[] = {"-fcolor-diagnostics",
                             "-fconst-strings",
                             "-fcxx-exceptions",
                             "-fdiagnostics-show-option",
                             "-fdeprecated-macro",
                             "-fexceptions",
                             "-fno-common",
                             "-fwritable-strings",
                             "-MP",
                             "-nostdinc++",
                             "-pedantic",
                             "-pedantic-errors",
                             "-w",
                             "-sys-header-deps",
                             "-v",
                             DebugSwitch};
  for (StringRef IgnoredArg : IgnoredArgs)
    if (Arg == IgnoredArg)
      return true;
  return Arg.startswith("-emit-llvm") || Arg.startswith(DebugOnlySwitch) ||
         Arg.startswith("-W");
}

static StringRef getNextArg(llvm::SmallVectorImpl<std::string> &Args,
                            std::string *&I, int FileNameIndex) {
  while (I < Args.end()) {
    StringRef Arg = (*I);
    bool IsFileName = I - Args.begin() == FileNameIndex;
    ++I;
    if (IsFileName)
      continue;
    if (isIgnoredValueArg(Arg)) {
      ++I;
      continue;
    }
    if (isIgnoredArg(Arg))
      continue;
    return Arg;
  }
  return "";
}

namespace {
class ArgsHistory {
  ArgsHistory(const ArgsHistory &) = delete;
  void operator=(const ArgsHistory &) = delete;

  llvm::SmallVector<std::string, 64> PreviousArgs;
  llvm::SmallVector<std::string, 64> CurrentArgs;
  int PreviousFileNameIndex = 0;
  int CurrentFileNameIndex = 0;

public:
  SmallString<1024> CurPath;
  ArgsHistory() { llvm::sys::fs::current_path(CurPath); }
  void push_back(const std::string &Arg) { CurrentArgs.push_back(Arg); }
  void markFileName() { CurrentFileNameIndex = CurrentArgs.size(); }
  StringRef getFileName() {
    assert(CurrentArgs.size());
    if (!CurrentFileNameIndex)
      return "-";
    return CurrentArgs[CurrentFileNameIndex];
  }
  void makeFileNameAbsolute() {
    SmallString<1024> FileName(getFileName());
    if (FileName == "-")
      return;
    llvm::sys::fs::make_absolute(FileName);
    CurrentArgs[CurrentFileNameIndex].assign(FileName.data(), FileName.size());
  }
  void getCharArray(llvm::SmallVector<const char *, 256> &Argv) {
    for (const std::string &Arg : CurrentArgs)
      Argv.push_back(Arg.c_str());
  }

  bool changed();
  void clear(bool Reset);
  bool isFirstRun() { return !PreviousArgs.size(); }
};
}

bool ArgsHistory::changed() {
  if (!PreviousArgs.size() || !CurrentArgs.size())
    return false;

  auto PreviousI = PreviousArgs.begin();
  auto CurrentI = CurrentArgs.begin();
  while (true) {
    StringRef PreviousArg =
        getNextArg(PreviousArgs, PreviousI, PreviousFileNameIndex);
    StringRef CurrentArg =
        getNextArg(CurrentArgs, CurrentI, CurrentFileNameIndex);
    if (PreviousArg == CurrentArg) {
      if (PreviousArg == "")
        break;
    } else {
#ifndef NDEBUG
#if 1
      llvm::outs() << "note: compilation arguments changed, first "
                      "difference:\n'"
                   << PreviousArg << "' => '" << CurrentArg << "'\n";
#else
      llvm::outs() << "note: compilation arguments changed, previous args:\n'";
      for (StringRef Arg : PreviousArgs)
        llvm::outs() << Arg << " ";
      llvm::outs() << "\nnote: current args:\n'";
      for (StringRef Arg : CurrentArgs)
        llvm::outs() << Arg << " ";
      llvm::outs() << "\n";
#endif
#endif
      return true;
    }
  }
  return false;
}

void ArgsHistory::clear(bool Reset) {
  if (Reset) {
    PreviousArgs.clear();
    PreviousFileNameIndex = 0;
  } else {
    PreviousArgs = CurrentArgs;
    PreviousFileNameIndex = CurrentFileNameIndex;
  }
  CurrentArgs.clear();
  CurrentFileNameIndex = 0;
}

static bool nextArgNotInput(StringRef Arg) {
  StringRef Args[] = {"-internal-isystem",
                      "-o",
                      "-I",
                      "-x",
                      "-coverage-file",
                      "-internal-externc-isystem",
                      "-main-file-name",
                      "-target-abi",
                      "-target-feature",
                      "-triple",
                      "-ferror-limit",
                      "-fmessage-length",
                      "-ftype-visibility",
                      "-fvisibility"};
  for (StringRef S : Args)
    if (Arg == S)
      return true;
  return false;
}

// 0 success, 1 failure, 2 compile with clang
static int performCompile(ArgsHistory &AH, clang::CachingCompiler &CC) {
  std::string WorkingDir;
  if (!std::getline(std::cin, WorkingDir))
    return 0;
  if (chdir(WorkingDir.c_str())) {
    CC.printServerId(llvm::errs()) << "Could not change directory to '"
                                   << WorkingDir << "'.\n";
    return 1;
  }
  std::string arg;
  llvm::SmallVector<std::string, 64> Args;
  while (std::getline(std::cin, arg)) {
    if (!arg.size())
      break;
    if (arg[0] == '$')
      arg = (AH.CurPath + arg.substr(1)).str();
    Args.push_back(arg);
  }

  if (Args.size()) {
    SmallString<1024> FileName(Args[Args.size() - 1]);
    llvm::sys::fs::make_absolute(FileName);
    if (US->isDoNotZap(FileName)) {
#ifndef NDEBUG
      llvm::outs() << "note: compiling without caching due to DoNotZap '"
                   << FileName << "'.\n";
#endif
      return 2;
    }
  }
  bool ResetFlag = false;
  bool SkipNextArg = false;
  bool NextArgNotInput = false;
  for (StringRef Arg : Args) {
    if (SkipNextArg) {
      SkipNextArg = false;
      continue;
    } else if (Arg == ResetSwitch) {
      llvm::outs() << "note: server reset requested.\n";
      ResetFlag = true;
    } else if (Arg == "-include-pch" || Arg == "-include-pth" ||
               Arg.startswith("-stack-protector")) {
      SkipNextArg = true;
      continue;
    } else if (Arg == "c++-cpp-output") {
      // ccache uses -x c++-cpp-output after preprocessing.
      Arg = "c++";
    } else if (nextArgNotInput(Arg)) {
      NextArgNotInput = true;
    } else if (NextArgNotInput) {
      NextArgNotInput = false;
    } else if (Arg[0] != '-') {
      // Must be main file name.
      AH.markFileName();
    }
    AH.push_back(Arg);
  }
  bool Success;
  if (ResetFlag) {
    CC.reset(false);
    Success = true;
  } else {
    if (AH.changed())
      CC.reset(false);
    AH.makeFileNameAbsolute();
    llvm::SmallVector<const char *, 256> Argv;
    AH.getCharArray(Argv);
    parseDebugFlags(Argv);
    Success = CC.compile(WorkingDir, Argv);
    if (!Success && !AH.isFirstRun() && AH.getFileName() != "-") {
      CC.printDiags(false);
      CC.clearDiags();
      CC.reset(true);
      Success = CC.compile(WorkingDir, Argv);
    }
    CC.printDiags(true);
    CC.clearDiags();
  }
  AH.clear(ResetFlag);
  // Print timers after every compilation, not at server shutdown.
  llvm::TimerGroup::printAll(llvm::errs());
  return !Success;
}

// Return true on success.
static bool runServerMode(Server &Srv, clang::CachingCompiler &CC,
                          const char *BinDir) {
  // Run in server mode.
  ArgsHistory AH;
  bool Success = true;
  bool OutOfMemory = false;
  bool FirstRun = true;
  do {
    if (!FirstRun && !Srv.disconnect()) {
      Success = false;
      break;
    }
    FirstRun = false;
    if (!Srv.connect()) {
      Success = false;
      break;
    }
    int RetCode = performCompile(AH, CC);
    Success = RetCode != 1;
    if (chdir(BinDir))
      return false;
    OutOfMemory = CC.isOutOfMemory();
    if (!Srv.writeExitCode(RetCode))
      Success = false;
  } while (Success && !OutOfMemory);
  Srv.done();
  return Success;
}

static bool runCommandFileMode(clang::CachingCompiler &CC,
                               const char *FileName) {
  if (!freopen(FileName, "r", stdin)) {
    llvm::errs() << "Could not open '" << FileName << "'.\n";
    return false;
  }
  ArgsHistory AH;
  bool Success = true;
  while (Success && !feof(stdin)) {
    Success = !performCompile(AH, CC);
    llvm::errs() << "##########################################\n\n";
  }
  fclose(stdin);
  return Success;
}

int main(int argc, const char *argv[]) {
  disableStreamsBuffering();
  const char *Argv0 = GetExePath(argv[0]);
  llvm::sys::PrintStackTraceOnErrorSignal(Argv0);
  llvm::PrettyStackTraceProgram X(argc, argv);

  // Initialize targets first, so that --version shows registered targets.
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  llvm::install_fatal_error_handler(LLVMErrorHandler);

  llvm::ArrayRef<const char *> Argv(argv, argc);
  bool ServerMode = (Argv.size() == 4);
  int ServerId;
  int FdInOut;
  int FdErr;
  if (ServerMode) {
    ServerId = getIntOption(Argv, 1);
    FdInOut = getIntOption(Argv, 2);
    FdErr = getIntOption(Argv, 3);
  } else {
    ServerId = 0;
    FdInOut = -1;
    FdErr = -1;
  }
  Server Srv(ServerId, FdInOut, FdErr);
  SmallString<1024> BinDir = llvm::sys::path::parent_path(Argv0);
  clang::CachingCompiler CC(ServerId, Argv0,
                            (void *)(intptr_t)LLVMErrorHandler);
  SignalHandlerData Data = {&Srv, &CC};
  llvm::sys::AddSignalHandler(signalHandler, &Data);

  bool Success = false;
  if (ServerMode)
    Success = runServerMode(Srv, CC, BinDir.c_str());
  else if (argc == 2)
    Success = runCommandFileMode(CC, argv[1]);
  else
    llvm::errs() << "No server id nor response file provided, nothing to do.\n";

  // Our error handler depends on the Diagnostics object, which we're
  // potentially about to delete. Uninstall the handler now so that any
  // later errors use the default handling behavior instead.
  llvm::remove_fatal_error_handler();

  // Managed static deconstruction. Useful for making things like
  // -time-passes usable.
  llvm::llvm_shutdown();

  return !Success;
}
