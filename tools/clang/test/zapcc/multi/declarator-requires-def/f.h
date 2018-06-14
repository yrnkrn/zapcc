#pragma once
struct Bug358A {
  ~Bug358A();
};
struct Bug358B : Bug358A {
  ~Bug358B();
};
struct Bug358C : Bug358B {};
struct Bug358MSP430Subtarget;
struct Bug358MSP430Subtarget {
  Bug358C memberVar;
};

struct Bug358A1 {
  Bug358A1(Bug358A1 &);
  Bug358A1();
};
struct Bug358B1 : Bug358A1 {
  Bug358B1();
};
struct Bug358MSP430TargetMachine : Bug358B1 {
  Bug358MSP430TargetMachine();
  Bug358MSP430Subtarget Subtarget;
};
