class AttributeUsedClass {
  void dump(bool);
  void dump() __attribute__((used)) { dump(false); }
};
