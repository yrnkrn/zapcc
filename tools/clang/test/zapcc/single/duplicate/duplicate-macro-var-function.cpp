// RUN: %zap_duplicate
// Bug 108
#define SEL_INLINE inline
SEL_INLINE void y1();
#define SEL_real double
int main() {
  SEL_real y1;
  y1++;
}
