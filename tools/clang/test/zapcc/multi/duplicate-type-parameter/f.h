#pragma once
template <typename> struct DuplicateTypeParameter { };
#define DUPLICATETYPEPARAMETER template <typename T> struct DuplicateTypeParameter<T[1]> {};
