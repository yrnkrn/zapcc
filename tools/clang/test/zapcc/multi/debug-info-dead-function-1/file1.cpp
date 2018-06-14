#include "f.h"
struct DIDF_ByteQueueNode {
  int CurrentSize() { return m_head; }
  DIDF_ByteQueueNode *next;
  int m_head;
};

DIDF_ByteQueue::~DIDF_ByteQueue() {}

int DIDF_ByteQueue::CurrentSize() {
  int size;
  for (DIDF_ByteQueueNode *current; current; current = current->next)
    size = current->CurrentSize();
  return size;
}
