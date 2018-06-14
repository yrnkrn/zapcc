#pragma once
void operator delete(void *)__attribute__((alias("ShimCppDelete")));
