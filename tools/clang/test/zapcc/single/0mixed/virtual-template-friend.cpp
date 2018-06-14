// RUN: %zap_compare_object
#include "virtual-template-friend.h"
struct JustID {
  static int ID;
};
TroubleID<JustID> P;
int JustID::ID;
int main() { }
