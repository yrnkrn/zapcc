#pragma once

template <typename> struct TemplatedStructA {};
template <typename T> struct TemplatedStructB : TemplatedStructA<T> {
  TemplatedStructA<int> A;
  TemplatedStructB() : TemplatedStructA<T>() {}
};
