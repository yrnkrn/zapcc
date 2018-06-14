class Bug335Class1 {
public:
  virtual ~Bug335Class1();
};
class Bug335Class2 : Bug335Class1 {};
class Bug335Class3 : Bug335Class2 {};
