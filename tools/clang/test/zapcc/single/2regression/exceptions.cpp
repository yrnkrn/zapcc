// RUN: %zap_compare_object
// RUN: %zap_compare_object
struct stype { ~stype() {} };
void t() { stype s; throw 1; }

int main() {
  try { t(); }
  catch (...) {}
}
