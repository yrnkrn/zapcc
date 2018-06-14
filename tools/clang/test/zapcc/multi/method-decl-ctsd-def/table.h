#pragma once
struct Bug1648Subscriptor {
  virtual ~Bug1648Subscriptor();
};
template <int, typename> class Bug1648Table;
template <typename T> class Bug1648Table<2, T> : Bug1648Subscriptor {};
