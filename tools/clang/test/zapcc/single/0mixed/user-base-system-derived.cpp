// RUN: %zap_compare_object
#include "user-base-system-derived.h"
struct UserBase {
  virtual ~UserBase() {}
};
struct UserBaseSystemDerived : UserBase {};
int main() { 
  new UserBaseSystemDerived;
}
