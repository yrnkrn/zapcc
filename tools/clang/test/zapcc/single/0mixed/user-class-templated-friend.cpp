// RUN: %zap_compare_object
// Bug 289
#include "user-class-templated-friend.h"
struct s {};
int main() {
  UserClassTemplatedFriend<s> m;
}
