// RUN: %zapcc_reset
// RUN: %clangxx -c %s -O -o %t -target x86_64-pc-linux -mllvm -debug-pass=Structure 2> %t.clang.txt
// RUN: %zapccxx -c %s -O -o %t -target x86_64-pc-linux -mllvm -debug-pass=Structure 2> %t.zap.txt
// RUN: diff -u %t.clang.txt %t.zap.txt
// Bug 1997
int main() {}
