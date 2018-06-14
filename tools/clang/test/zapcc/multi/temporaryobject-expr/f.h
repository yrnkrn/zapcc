#pragma once
struct Bug393Struct {
  Bug393Struct(unsigned, unsigned);
};
struct Bug393TemplatedMember {
  Bug393TemplatedMember() { read<unsigned>(0); }
  template <typename T> void read(unsigned) { Bug393Struct(0, 0); }
};
