#pragma once
template <typename> struct Bug1105TypeIdHelper { static bool b; };
template <typename T> bool Bug1105TypeIdHelper<T>::b;
void foo1() { Bug1105TypeIdHelper<char>::b; }
