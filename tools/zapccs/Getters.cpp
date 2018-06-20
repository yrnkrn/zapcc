// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "CachingCompiler.h"

#include "llvm/IR/DebugInfoMetadata.h"

#include "../lib/CodeGen/CodeGenModule.h"
#include "clang/CodeGen/ModuleBuilder.h"
#include "clang/Frontend/CompilerInstance.h"

namespace clang {

CodeGen::CodeGenModule *getBuilder(CompilerInstance *CI) {
  if (auto *CodeGen = CI->CodeGen.get())
    return CodeGen->getBuilder();
  else
    return nullptr;
}

CodeGen::CGDebugInfo *getModuleDebugInfo(CompilerInstance *CI) {
  if (CodeGen::CodeGenModule *Builder = getBuilder(CI))
    return Builder->getModuleDebugInfo();
  else
    return nullptr;
}

llvm::Module *getModule(CompilerInstance *CI) {
  // Builder Module may be dead, if an error occured.
  // Yet ExecuteCompilerInvocation may return success if -Xclang -verify
  // and the error was expected.
  CodeGenerator *CodeGen = CI->CodeGen.get();
  if (CodeGen)
    return CodeGen->GetModule();
  else
    return nullptr;
}

llvm::DICompileUnit *getCU(llvm::Module &M) {
  llvm::NamedMDNode *CU_Nodes = M.getNamedMetadata("llvm.dbg.cu");
  if (CU_Nodes)
    return cast<llvm::DICompileUnit>(CU_Nodes->getOperand(0));
  return nullptr;
}

} // end namespace
