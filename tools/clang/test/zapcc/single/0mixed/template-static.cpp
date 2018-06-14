// RUN: %zap_compare_object
#include "template-static.h"
struct UserStruct {
  static int anchor;
};
int UserStruct::anchor;
int main() { 
  SystemStruct<UserStruct>().justFunc();
}
