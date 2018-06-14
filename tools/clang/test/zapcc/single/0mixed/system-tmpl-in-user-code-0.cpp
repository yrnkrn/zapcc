// RUN: %zap_compare_object
#include "system-tmpl-in-user-code-0.h"
namespace SystemTemplateInUserCodeNS {
template <typename, typename> class SystemTemplateInUserCodeClass;
}
int main() {
  SystemTemplateInUserCodeNS::SystemTemplateInUserCodeClass<int> v;
}
