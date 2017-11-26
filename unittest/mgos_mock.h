#ifndef __MGOS_MOCK_H
#define __MGOS_MOCK_H

/* Some functions mocked from MGOS, so we can run unit tests standalone.
 */

#include <stdio.h>
#include <string.h>

enum cs_log_level {
  LL_NONE = -1,
  LL_ERROR = 0,
  LL_WARN = 1,
  LL_INFO = 2,
  LL_DEBUG = 3,
  LL_VERBOSE_DEBUG = 4,

  _LL_MIN = -2,
  _LL_MAX = 5,
};

int log_print_prefix(enum cs_log_level l, const char *func, const char *file);

#define LOG(l, x)                                                    \
  do {                                                               \
    if (log_print_prefix(l, __func__, __FILE__)) printf x; \
    printf("\r\n"); \
  } while (0)

#endif // __MGOS_MOCK_H
