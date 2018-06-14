// RUN: %zap_compare_object
// Bug 415
#include "typeid-reset.h"
struct Bug415LocalAuthorizerProcess {};
int main() {
  Bug415LocalAuthorizerProcess L;
  (void)typeid(&L);
}
