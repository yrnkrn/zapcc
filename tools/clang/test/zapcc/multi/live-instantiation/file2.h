#include "file1.h"
struct Bug530SpillPlacement : Bug530MachineFunctionPass {
  int *nodes;
  Bug530SpillPlacement() { releaseMemory(); }
  void releaseMemory();
};
