#pragma once
template <class T> struct Bug556Disposable : T {
  Bug556Disposable(T p1) { this->swap1(); }
};
