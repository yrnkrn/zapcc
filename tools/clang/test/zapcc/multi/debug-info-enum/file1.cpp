class Bug843optional;
#include "file1.h"
#include "file2.h"
class Bug843ReplSettings {
  struct string {
    ~string();
  };
  Bug843BackgroundSync::IndexPrefetchConfig b;
  string c;
} a;
