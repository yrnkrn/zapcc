#pragma once
void Bug2359basic_string(char *);
namespace std {
struct type_info {
  char *name;
};
}
struct Bug2359ParameterList {
  virtual ~Bug2359ParameterList();
};
void Bug2359any_cast() { Bug2359basic_string(typeid(Bug2359ParameterList).name); }
