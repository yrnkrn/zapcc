#pragma once
template <typename> struct Bug2572_standard_migrator {
  Bug2572_standard_migrator() {}
  static Bug2572_standard_migrator object;
};
template <typename T> Bug2572_standard_migrator<T> Bug2572_standard_migrator<T>::object;
struct Bug2572_allocation_strategy {
  virtual void alloc();
  template <typename T> void construct() { Bug2572_standard_migrator<T>::object; }
};
struct collection_mutation {
  collection_mutation();
};
collection_mutation::collection_mutation() { Bug2572_allocation_strategy data; }
