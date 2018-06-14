// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCC_CACHINGCOMPILER_H
#define ZAPCC_CACHINGCOMPILER_H

#include "raw_colored_svector_ostream.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallString.h"

#include <ctime>
#include <memory>
#include <string>

namespace llvm {
class CachingPassManager;
class raw_ostream;
}

namespace clang {
class CompilerInstance;
class DependencyMap;
class DiagnosticConsumer;
class FileEntry;
class IdentifierInfo;
class Preprocessor;
class ZapccConfig;

class CachingCompiler {
  CachingCompiler(CachingCompiler &) = delete;
  CachingCompiler &operator=(CachingCompiler &) = delete;

  std::unique_ptr<ZapccConfig> ZC;
  std::unique_ptr<CompilerInstance> CI;
  std::unique_ptr<llvm::CachingPassManager> CPM;
  std::unique_ptr<DependencyMap> DM;
  std::unique_ptr<DiagnosticConsumer> DiagsClient;
  llvm::SmallString<4096> DiagsString;
  llvm::raw_colored_svector_ostream DiagsStream;
  bool PrintCommandsTxt;
  llvm::SmallString<1024> CommandsTxtName;
  int ServerId;
  const char *Argv0;
  void *MainAddr;
  std::time_t LastCompileEndTime = 0;

  void init();
  void unload();
  void PreprocessorCreatedCallBack(Preprocessor &PP);

public:
  CachingCompiler(int Id, const char *Argv0, void *MainAddr);
  ~CachingCompiler();
  void reset(bool IsError);
  void printCommandsTxtName(llvm::StringRef What);
  llvm::raw_ostream &printServerId(llvm::raw_ostream &OS);
  bool compile(llvm::StringRef WorkingDir,
               llvm::MutableArrayRef<const char *> Argv);
  void included(const FileEntry *Includee);
  bool isOutOfMemory();
  void printDiags(bool AlwaysPrint);
  void clearDiags() { DiagsString.clear(); }
};
}

#endif // ZAPCC_CACHINGCOMPILER_H
