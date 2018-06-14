#include "f.h"
struct LocalByteQueueNode {
  int CurrentSize() { return m_head; }
  LocalByteQueueNode *next;
  int m_head;
};

DIDF2_ByteQueue::~DIDF2_ByteQueue() {}

int DIDF2_ByteQueue::CurrentSize() {
  int size;
  for (LocalByteQueueNode *current; current; current = current->next)
    size = current->CurrentSize();
  return size;
}
