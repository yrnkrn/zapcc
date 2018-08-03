#pragma once
struct Issue21A {
  Issue21A() { tm = 0; }
  static thread_local int tm;
};
