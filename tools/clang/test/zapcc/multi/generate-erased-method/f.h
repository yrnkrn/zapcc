#pragma once
struct DIDF2_BufferedTransformation {
  virtual ~DIDF2_BufferedTransformation();
  virtual int MaxRetrievable();
};

struct DIDF2_ByteQueueNode;

struct DIDF2_ByteQueue : DIDF2_BufferedTransformation {
  ~DIDF2_ByteQueue();
  int MaxRetrievable() { return CurrentSize(); }
  int CurrentSize();
};
