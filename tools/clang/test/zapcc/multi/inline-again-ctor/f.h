#pragma once
struct Bug413Mat {
  Bug413Mat(const Bug413Mat &);
};
struct Bug413_InputArray {
  Bug413Mat getMat();
  void *obj;
};
void Bug413copySize();
inline Bug413Mat Bug413_InputArray::getMat() { return *(Bug413Mat *)obj; }
Bug413Mat::Bug413Mat(const Bug413Mat &) { Bug413copySize(); }
