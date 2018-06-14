#pragma once
template <typename> struct Bug1919smart_ref {
  Bug1919smart_ref();
  void operator*();
};
void Bug1919foo() { Bug1919smart_ref<int> current_fees; }
