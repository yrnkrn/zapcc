#pragma once
struct Bug1045_cpp_dec_float {
  Bug1045_cpp_dec_float();
  ~Bug1045_cpp_dec_float();
  void foo() { rd_string(); }
  void rd_string() { Bug1045_cpp_dec_float h; }
};
struct Bug1045_f {
  Bug1045_f() { Bug1045_cpp_dec_float().foo(); }
};
