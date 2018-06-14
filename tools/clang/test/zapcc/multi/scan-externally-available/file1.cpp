#include "f.h"
class Bug388nonfinite_num_get : Bug388num_get<char> {};
extern "C" int puts(const char*);
int main() { 
  Bug388locale default_locale(new Bug388nonfinite_num_get);
  puts("scan-externally-available");
}
