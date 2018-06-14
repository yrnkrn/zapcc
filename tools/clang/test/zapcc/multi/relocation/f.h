#pragma once
struct Bug810Pass {
  virtual ~Bug810Pass();
  virtual void createPrinterPass();
};
class Bug810FunctionPass : Bug810Pass {
  void createPrinterPass();
};
