#pragma once
struct exception {
  virtual ~exception() = 0;
};
struct error_info_injector : exception {};
