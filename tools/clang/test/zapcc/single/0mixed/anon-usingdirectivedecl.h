#pragma once
namespace {
struct Bug738char_traits;
}
#include "anon-usingdirectivedecl-2.h"
template <typename, typename> class Bug738basic_string;
template <typename, typename = Bug738char_traits> class Bug738basic_string;
template <typename _CharT> void Bug738foo(Bug738basic_string<_CharT>);
template <typename _Tp> struct Bug738unique_ptr {
  template <typename> _Tp *__test();
  ~Bug738unique_ptr() { delete __test<int>(); }
};
class Bug738Scanner;
struct Bug738Stream {
  Bug738Stream();
  Bug738unique_ptr<Bug738Scanner> scanner;
};
