#pragma once
template <typename T> struct DestructorInDeletEexprPtr {
  DestructorInDeletEexprPtr() { delete (T *)(nullptr); }
};
struct DestructorInDeletEexprS {
  ~DestructorInDeletEexprS();
};
