// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCC_GETTERS_H
#define ZAPCC_GETTERS_H

namespace llvm {
class Module;
class DICompileUnit;
}

namespace clang {
class CompilerInstance;

namespace CodeGen {
class CGDebugInfo;
class CodeGenModule;
}

CodeGen::CodeGenModule *getBuilder(CompilerInstance *CI);
CodeGen::CGDebugInfo *getModuleDebugInfo(CompilerInstance *CI);
llvm::Module *getModule(CompilerInstance *CI);
llvm::DICompileUnit *getCU(llvm::Module &M);
}

#endif
