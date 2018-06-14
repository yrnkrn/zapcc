#pragma once
class Bug1834StochasticProcess {
  virtual unsigned size();
};
class Bug1834HestonProcess : Bug1834StochasticProcess {
  unsigned size();
};
