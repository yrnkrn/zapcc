// RUN: %zap_compare_object
#include "printer-123.h"
struct Paper : Templ3<Paper> {};
template <> struct Templ1<Paper> {};
int main() {
  Templ2<Paper>::Int I;
  Printer::print(nullptr);
}
