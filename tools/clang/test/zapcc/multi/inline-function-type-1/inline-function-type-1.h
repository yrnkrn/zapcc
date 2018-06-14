#pragma once
struct IFT1_ByteQueueNode;
struct IFT1_ByteQueue {
  virtual ~IFT1_ByteQueue();
  virtual bool AnyRetrievable() { return IsEmpty(); }
  bool IsEmpty();
  IFT1_ByteQueueNode *p_head;
};
