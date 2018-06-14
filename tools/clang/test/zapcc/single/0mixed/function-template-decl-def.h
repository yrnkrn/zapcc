#pragma once
template <typename T> T doublerImpl(T x);
char doubler(char x) { return doublerImpl(x); }
