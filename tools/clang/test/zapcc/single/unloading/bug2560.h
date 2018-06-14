#pragma once
template <typename> struct Bug2560pair { void foo(); };
void emplace() { 
  Bug2560pair<int> *p;
  p->foo();
}
