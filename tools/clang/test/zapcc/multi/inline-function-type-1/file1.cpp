#include "inline-function-type-1.h"
struct IFT1_ByteQueueNode {
  bool something;
  int mustBeSecond;
};

IFT1_ByteQueue::~IFT1_ByteQueue() {}

bool IFT1_ByteQueue::IsEmpty()  {
	return p_head->mustBeSecond;
}

extern "C" int puts(const char *s);
int main() {
  puts("inline-function-type-1");
}
