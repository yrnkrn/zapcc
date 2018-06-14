// RUN: %zap_compare_object
// RUN: %zap_compare_object
int main() {
try {
throw 123;
}
catch (int d) {
}
}
