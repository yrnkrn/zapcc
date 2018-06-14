#include "f.h"
template class ete2<int>;
int main() { 
  ete2<int>().f();
  puts("extern-implicit-template-explicit"); 
}
