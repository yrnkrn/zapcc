// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "UnloadSelector.h"
#include "ZapccConfig.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Path.h"

#include "clang/Basic/FileManager.h"

namespace clang {
using llvm::SmallString;
using llvm::StringRef;

static bool WildMatch6(const char *pat, const char *str) {
  const char *s;
  const char *p;
  bool star = false;

loopStart:
  for (s = str, p = pat; *s; ++s, ++p) {
    switch (*p) {
    case '?':
      if (*s == '.')
        goto starCheck;
      break;
    case '*':
      star = true;
      str = s, pat = p;
      do {
        ++pat;
      } while (*pat == '*');
      if (!*pat)
        return true;
      goto loopStart;
    default:
      if (*s != *p)
        goto starCheck;
      break;
    }
  }
  while (*p == '*')
    ++p;
  return (!*p);

starCheck:
  if (!star)
    return false;
  str++;
  goto loopStart;
}

static void normalizeSlashes(SmallString<1024> &Output) {
  for (char &c : Output)
    if (c == '/' || c == '\\')
      c = '/';
    else
      c = tolower(c);
}

class WildRegex {
  enum class MatchType { EndsWith, Find, Same, Wild };
  struct Match {
    std::string Pattern;
    MatchType Type;
  };
  SmallVector<Match, 16> YesMatches;
  SmallVector<Match, 16> NoMatches;
  bool match(StringRef Name, bool No);

public:
  WildRegex(const ZapccConfig::PatternsType &Patterns);
  ~WildRegex() {}
  bool match(llvm::StringRef Name) {
    return match(Name, false) && !match(Name, true);
  }
};

WildRegex::WildRegex(const ZapccConfig::PatternsType &Patterns) {
  YesMatches.reserve(Patterns.size());
  for (StringRef Pattern : Patterns) {
    bool No = Pattern.startswith("!");
    if (No)
      Pattern = Pattern.drop_front();
    SmallVectorImpl<Match> &Matches = No ? NoMatches : YesMatches;
    SmallString<1024> NormalizedPattern(Pattern);
    normalizeSlashes(NormalizedPattern);
    StringRef Pat(NormalizedPattern);
    unsigned Questions = Pat.count('?');
    if (Questions == 0) {
      unsigned Stars = Pat.count('*');
      if (Stars == 0) {
        Matches.push_back({Pat, MatchType::Same});
        continue;
      } else if (Stars == 1 && Pat.front() == '*') {
        Matches.push_back({Pat.drop_front(), MatchType::EndsWith});
        continue;
      } else if (Stars == 2 && Pat.front() == '*' && Pat.back() == '*') {
        Matches.push_back({Pat.drop_front().drop_back(), MatchType::Find});
        continue;
      }
    }
    Matches.push_back({Pat, MatchType::Wild});
  }
}

bool WildRegex::match(llvm::StringRef Name, bool No) {
  SmallString<1024> FileName(Name);
  SmallVectorImpl<Match> &Matches = No ? NoMatches : YesMatches;
  for (auto &Match : Matches) {
    switch (Match.Type) {
    case MatchType::EndsWith:
      if (StringRef(Name).endswith(Match.Pattern))
        return true;
      break;
    case MatchType::Find:
      if (Name.find(Match.Pattern) != StringRef::npos)
        return true;
      break;
    case MatchType::Same:
      if (Name == Match.Pattern)
        return true;
      break;
    case MatchType::Wild:
      if (WildMatch6(Match.Pattern.c_str(), FileName.c_str()))
        return true;
      break;
    }
  }
  return false;
}

UnloadSelector::~UnloadSelector() {
  delete MutableRegex;
  delete DoNotZapRegex;
}

void UnloadSelector::reset(ZapccConfig &ZC) {
  assert(!MutableRegex);
  MutableRegex = new WildRegex(ZC.getPatterns("[Mutable]"));
  assert(!DoNotZapRegex);
  DoNotZapRegex = new WildRegex(ZC.getPatterns("[DoNotZap]"));
}

static bool isRegex(WildRegex *Regex, llvm::StringRef FileName) {
  assert(Regex);
  SmallString<1024> Name(FileName);
  llvm::sys::path::remove_dots(Name, true);
  normalizeSlashes(Name);
  return Regex->match(Name);
}

bool UnloadSelector::isMutable(llvm::StringRef FileName) {
  return isRegex(MutableRegex, FileName);
}

bool UnloadSelector::isDoNotZap(llvm::StringRef FileName) {
  return isRegex(DoNotZapRegex, FileName);
}

llvm::ManagedStatic<UnloadSelector> US;
}
