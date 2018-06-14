#pragma once
struct Bug2557kind {
  template <typename T> Bug2557kind(T) {}
  static Bug2557kind map;
};
