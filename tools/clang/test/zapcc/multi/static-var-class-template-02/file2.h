#pragma once
template <class a> struct Bug1045_d {
  static void c() { (void)e; }
  static a e;
};
template <class a> a Bug1045_d<a>::e;
