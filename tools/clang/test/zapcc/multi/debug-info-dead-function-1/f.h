#pragma once
struct DIDF_BufferedTransformation {
  virtual ~DIDF_BufferedTransformation();
  virtual int MaxRetrievable();
};

struct DIDF_ByteQueueNode;

struct DIDF_ByteQueue : DIDF_BufferedTransformation {
  ~DIDF_ByteQueue();
  int MaxRetrievable() { return CurrentSize(); }
  int CurrentSize();
};
