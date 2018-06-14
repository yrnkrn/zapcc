// RUN: %zap_compare_object
// RUN: %zap_compare_object
template <typename T> T n = T(5);
template <typename T> constexpr T pi = T(3.1415926535897932385);

int main() {
  n<int> = 10;
  pi<double>;
}
