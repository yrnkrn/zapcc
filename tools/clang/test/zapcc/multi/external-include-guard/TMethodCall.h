#ifndef ROOT_TMethodCall
#define ROOT_TMethodCall
#ifndef ROOT_TInterpreter
#include "TInterpreter.h"
#endif
template <typename T> void Bug2095SetParams() {
  Bug2095CallFunc_SetArguments<int>();
}
#endif
