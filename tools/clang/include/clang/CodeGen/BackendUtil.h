//===--- BackendUtil.h - LLVM Backend Utilities -----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_CODEGEN_BACKENDUTIL_H
#define LLVM_CLANG_CODEGEN_BACKENDUTIL_H

#include "clang/Basic/LLVM.h"
#include "llvm/IR/ModuleSummaryIndex.h"
#include <memory>

namespace llvm {
  class BitcodeModule;
  template <typename T> class Expected;
  class Module;
  class TargetLibraryInfoImpl;
  class TargetMachine;
  class Triple;
  namespace legacy {
    class FunctionPassManager;
    class PassManager;
    class PassManagerBase;
  }
  class MemoryBufferRef;
}

namespace clang {
  class DiagnosticsEngine;
  class EmitAssemblyHelper;
  class HeaderSearchOptions;
  class CodeGenOptions;
  class TargetOptions;
  class LangOptions;

  enum BackendAction {
    Backend_EmitAssembly,  ///< Emit native assembly files
    Backend_EmitBC,        ///< Emit LLVM bitcode files
    Backend_EmitLL,        ///< Emit human-readable LLVM assembly
    Backend_EmitNothing,   ///< Don't emit anything (benchmarking mode)
    Backend_EmitMCNull,    ///< Run CodeGen, but don't emit anything
    Backend_EmitObj        ///< Emit native object files
  };

  void EmitBackendOutput(DiagnosticsEngine &Diags, const HeaderSearchOptions &,
                         const CodeGenOptions &CGOpts,
                         const TargetOptions &TOpts, const LangOptions &LOpts,
                         const llvm::DataLayout &TDesc, llvm::Module *M,
                         BackendAction Action,
                         std::unique_ptr<raw_pwrite_stream> OS);

  class CachingPassHelper {
  public:
    CachingPassHelper(DiagnosticsEngine &Diags,
                      const HeaderSearchOptions &HeaderSearchOpts,
                      const CodeGenOptions &CGOpts,
                      const clang::TargetOptions &TOpts,
                      const LangOptions &LOpts, llvm::Module *M);
    ~CachingPassHelper();
    llvm::legacy::PassManager *getPerModulePasses() const {
      return PerModulePasses;
    }
    llvm::legacy::FunctionPassManager *getPerFunctionPasses() const {
      return PerFunctionPasses;
    }
    llvm::TargetMachine *CreateTargetMachine(bool MustCreateTM);
    void CreatePasses();
    bool AddEmitPasses(llvm::legacy::PassManagerBase &CodeGenPasses,
                       BackendAction Action, raw_pwrite_stream &OS);

  private:
    EmitAssemblyHelper *EAH;
    llvm::legacy::PassManager *PerModulePasses;
    llvm::legacy::FunctionPassManager *PerFunctionPasses;
  };
  llvm::TargetLibraryInfoImpl *createTLII(llvm::Triple &TargetTriple,
                                          const CodeGenOptions &CodeGenOpts);

  void EmbedBitcode(llvm::Module *M, const CodeGenOptions &CGOpts,
                    llvm::MemoryBufferRef Buf);

  llvm::Expected<llvm::BitcodeModule>
  FindThinLTOModule(llvm::MemoryBufferRef MBRef);
}

#endif
