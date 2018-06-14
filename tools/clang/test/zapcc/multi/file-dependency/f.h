#pragma once
void Bug814foo3();
template <typename a> void Bug814call_once(a) {
  Bug814foo3();
}
