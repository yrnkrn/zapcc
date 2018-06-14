// RUN: %zapccxx -fsyntax-only %s %s > %t.txt 2>&1
// RUN: FileCheck %s -allow-empty < %t.txt 
// CHECK-NOT: error: 
template <class T>
void apply(T value) { foo(value); }
namespace gregorian {
class date {};
}
void foo(gregorian::date &x) {
}
int main() {
  apply(gregorian::date());
}
