#pragma once
struct Bug1045rd_string {
  Bug1045rd_string();
};
template <class a> struct Bug1045d {
  static a h;
};
template <class a> a Bug1045d<a>::h;
