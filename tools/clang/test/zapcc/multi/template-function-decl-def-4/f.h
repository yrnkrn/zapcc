#pragma once
template <typename T> void Bug678sort(T);
template <typename T> void Bug678sort(T) { (void)Bug678userfunc; }
