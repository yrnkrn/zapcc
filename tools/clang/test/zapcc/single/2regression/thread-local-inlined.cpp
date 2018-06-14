// RUN: %zap_compare_object
// RUN: %zap_compare_object
thread_local int i;
int main() {
  return i;
}
