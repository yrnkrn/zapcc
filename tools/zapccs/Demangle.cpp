// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "Demangle.h"
#include "DemangleGoogle.h"

#include "llvm/ADT/SmallString.h"

#include <string>

// Return demangled function name, supports function name up to 99 chars.
static llvm::StringRef demangleFunctionName(llvm::StringRef MangledName) {
  if ((MangledName.size() < 4) || (MangledName[0] != '_') ||
      (MangledName[1] != 'Z') || !isdigit(MangledName[2]))
    return MangledName;

  if (isdigit(MangledName[3]))
    return MangledName.substr(4, 10 * (MangledName[2] - '0') +
                                     (MangledName[3] - '0'));
  else
    return MangledName.substr(3, MangledName[2] - '0');
} // deMangleFunctionName

std::string demangle(llvm::StringRef name) {
  if (name.startswith("\x01"))
    name = name.drop_front();
  if (name.startswith("__"))
    name = name.drop_front();
  size_t lastAt = name.find_last_of("@");
  if (lastAt != llvm::StringRef::npos)
    name = name.drop_back(name.size() - lastAt);

  char buf[1024];
  llvm::SmallString<sizeof(buf)> Name(name);
#if 0
  // Try libcxxabi function.
  int status;
  size_t namesize = sizeof(buf);
  __cxa_demangle(Name.c_str(), buf, &namesize, &status);
  if (status == 0)
    return buf;
#endif
#if 1
  // Try google function.
  if (Demangle(Name.c_str(), buf, sizeof(buf)))
    return buf;
#endif
  // Failed, demangle function name.
  return demangleFunctionName(name);
}
