#pragma once
template <typename> struct Bug1962NLReader {
  template <typename = int> struct BinaryArgReader {};
  void Read() { BinaryArgReader<> args0; }
};
