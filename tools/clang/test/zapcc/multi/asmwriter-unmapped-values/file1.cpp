void unmmapedFoo();

struct UnmappedStruct {
  ~UnmappedStruct() { unmmapedFoo(); }
};

void UnmappedFoo() {
  UnmappedStruct A;
  UnmappedStruct B;
}
