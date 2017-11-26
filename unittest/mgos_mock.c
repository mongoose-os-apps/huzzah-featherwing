/* Some functions mocked from MGOS, so we can run unit tests standalone.
 */

#include "mgos_mock.h"

int log_print_prefix(enum cs_log_level l, const char *func, const char *file) {
  char ll_str[6];

  switch(l) {
    case LL_ERROR:
      strncpy(ll_str, "ERROR", sizeof(ll_str));
      break;
    case LL_WARN:
      strncpy(ll_str, "WARN", sizeof(ll_str));
      break;
    case LL_INFO:
      strncpy(ll_str, "INFO", sizeof(ll_str));
      break;
    case LL_DEBUG:
      strncpy(ll_str, "DEBUG", sizeof(ll_str));
      break;
    case LL_VERBOSE_DEBUG:
      strncpy(ll_str, "VERB", sizeof(ll_str));
      break;
    default: // LL_NONE
      return 0;
  }
  printf ("%-5s %-10s %-15s| ", ll_str, file, func);
  return 1;
}
