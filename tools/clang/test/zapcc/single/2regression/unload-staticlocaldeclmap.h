#pragma once
struct Bug2030IDBAny {
  ~Bug2030IDBAny();
};
struct Bug2030GCInfo {
  void (*m_nonTrivialFinalizer)();
};
void Bug2030index() {
  const Bug2030GCInfo gcInfo{Bug2030index};
  Bug2030IDBAny();
}
