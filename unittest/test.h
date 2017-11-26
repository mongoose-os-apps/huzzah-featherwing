#ifndef __TEST_H
#define __TEST_H

#include <stdlib.h>
#include <string.h>
#include "frozen/frozen.h"
#include "mgos_mock.h"

extern int test_failures;
extern int assert_count;

#define ASSERT(expr, errstr)               \
  do {                                     \
    if (!(expr)) {  \
      LOG(LL_ERROR, ("ASSERT FAIL: "errstr));  \
      test_failures++;                       \
    } \
    assert_count++; \
  } while (0)



int test_widget(void);
int test_screen(void);

#endif // __TEST_H
