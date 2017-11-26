#include "test.h"

int test_failures=0;
int assert_count=0;

int main() {
  test_widget();
  test_screen();

  if (test_failures) {
    LOG(LL_ERROR, ("%d test failures", test_failures));
    return -1;
  }
  LOG(LL_INFO, ("All tests passed, %d assertions OK", assert_count));
  return 0;
}
