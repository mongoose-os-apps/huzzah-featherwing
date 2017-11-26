#include "test.h"
#include "widget.h"

extern int _mgos_timers;

static int test_widget_create_from_file(void) {
  struct widget_t *w;

  char *fn = "data/TestWidget.json";
  LOG(LL_INFO, ("widget_create_from_file(%s)", fn));
  w = widget_create_from_file(fn);
  ASSERT(w, "widget_create_from_file()");
  ASSERT(w->x == 16, "'x' field is invalid");
  ASSERT(w->y == 16, "'x' field is invalid");
  ASSERT(w->w == 48, "'x' field is invalid");
  ASSERT(w->h == 48, "'x' field is invalid");

  LOG(LL_INFO, ("widget_set_timer()"));
  widget_set_timer(w, 1000);
  ASSERT(_mgos_timers==1, "timer not found");

  LOG(LL_INFO, ("widget_delete_timer()"));
  widget_delete_timer(w);
  ASSERT(_mgos_timers==0, "timers found");

  fn = "data/TestWidget-invalid.json";
  LOG(LL_INFO, ("widget_create_from_file(%s)", fn));
  w = widget_create_from_file(fn);
  ASSERT(!w, "invalid widget created");
  return 0;
}

int test_widget() {
  test_widget_create_from_file();

  LOG(LL_INFO, ("ensure we have no timers"));
  ASSERT(_mgos_timers==0, "timers found");

  return 0;
}
