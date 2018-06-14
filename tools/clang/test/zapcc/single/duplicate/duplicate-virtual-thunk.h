class VTClass1 {
  int someMember1;
  virtual bool foo() = 0;
};
class VTClass2 : virtual VTClass1 {
  int someMember2;
  bool foo() override { return false; }
};
class VTClass3 : virtual VTClass2 {
  bool foo() override;
};
