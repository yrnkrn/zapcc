// RUN: %zap_compare_object
// RUN: %zap_compare_object
template <class _T1, class _T2> struct pair {};
template <typename BucketT> struct DenseMapBase {
  BucketT *bucket = 0;
  void destroyAll() { ++bucket; }
};
template <typename KeyT> struct DenseMap : DenseMapBase<pair<int, int>> {
  ~DenseMap() { this->destroyAll(); }
};
struct Base {
  virtual ~Base();
};
struct SubClass : Base {
  SubClass();
  DenseMap<int> map;
};
SubClass::SubClass() {}
