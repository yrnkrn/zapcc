#pragma once
extern "C" int puts(const char *s);
struct ShouldNotBeDuplicatedType {
  ShouldNotBeDuplicatedType() {
    puts("system-variable");
  }
} ;
ShouldNotBeDuplicatedType ShouldNotBeDuplicated;
