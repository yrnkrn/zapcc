#pragma once
template <typename T> Bug1919smart_ref<T>::Bug1919smart_ref() { **this; }
template <typename T> void Bug1919smart_ref<T>::operator*() {}
