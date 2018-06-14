#pragma once
struct Paper;
struct Printer {
  static void print(Paper *) { }
};
template <typename T> struct Templ1;
template <typename T> struct Templ2 : Templ1<T> { typedef int Int; };
template <typename T> struct Templ3 { friend Templ2<T>; };
