#define GTEST_TEST_CLASS_NAME_(test_case_name, test_name)                      \
  test_case_name##_##test_name##_Test
#define TEST_P(test_case_name, test_name)                                      \
  class GTEST_TEST_CLASS_NAME_(test_case_name, test_name) {                    \
    static int gtest_registering_dummy_;                                       \
  };                                                                           \
  int GTEST_TEST_CLASS_NAME_(test_case_name,                                   \
                             test_name)::gtest_registering_dummy_;
#define BITMASK_CLASSNAME_(S) BitmaskTest##S
#define BITMASK_CLASSNAME(S) BITMASK_CLASSNAME_(S)
#define BITMASK_TEST_P_(C, T) TEST_P(C, T)
#define BITMASK_TEST_P(T) BITMASK_TEST_P_(BITMASK_CLASSNAME(INTEL_NO_MACRO_BODY), T)
BITMASK_TEST_P(Union)