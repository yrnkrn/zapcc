#pragma once
struct Bug436Date {
  Bug436Date();
  Bug436Date(long);
  Bug436Date operator+(long p1) const { return p1; }
  long serialNumber_;
};
