#pragma once
struct DIDD_results0 {};
struct DIDD_results1 {
  DIDD_results0 a;
  virtual ~DIDD_results1();
};
struct DIDD_results2 : virtual DIDD_results1 {};
struct DIDD_results3 : DIDD_results2 {};
