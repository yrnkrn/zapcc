#pragma once
extern "C" void Bug1010tc_malloc();
void a() __attribute__((alias("Bug1010tc_malloc")));
void b() __attribute__((alias("Bug1010tc_mallinfo")));
