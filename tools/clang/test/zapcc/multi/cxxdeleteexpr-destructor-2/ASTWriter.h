#pragma once
struct Bug1349BitstreamWriter {
  ~Bug1349BitstreamWriter();
};
struct Bug1349ASTWriterData;
template <typename T> struct Bug1349unique_ptr {
  ~Bug1349unique_ptr() { delete (T *)nullptr; }
};
class Bug1349ASTUnit {
  Bug1349unique_ptr<Bug1349ASTWriterData> WriterData;
  ~Bug1349ASTUnit();
};
