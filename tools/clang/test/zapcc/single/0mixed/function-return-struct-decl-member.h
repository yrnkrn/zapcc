template <typename T> struct StructWithTypenameOnly { T t; };
class ClassDeclaredInHeader;
StructWithTypenameOnly<ClassDeclaredInHeader> FunctionReturningTemplated();
