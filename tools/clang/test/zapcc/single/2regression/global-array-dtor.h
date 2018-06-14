#pragma once
struct GlobalArrayDtor {
  struct S {
    ~S();
  } s;
};
