#pragma once
template <typename Char> struct TemplateStaticUserClass {
  static void foo();
};
inline void format() { TemplateStaticUserClass<char> w; }
