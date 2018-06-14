void Bug1296foo2() {
#pragma omp taskloop
  for (int i = 0; i < 0; ++i)
    ;
}
