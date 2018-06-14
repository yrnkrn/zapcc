#pragma once
struct Bug526Value {
  Bug526Value(int *);
  virtual ~Bug526Value();
};
struct Bug526UnaryInstruction : Bug526Value {
  Bug526UnaryInstruction() : Bug526Value(&Op) {}
  ~Bug526UnaryInstruction();
  int Op;
};
struct Bug526AllocaInst : Bug526UnaryInstruction {
  Bug526AllocaInst();
  ~Bug526AllocaInst();
};
