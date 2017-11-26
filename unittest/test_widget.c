#include "test.h"
#include "widget.h"

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

  fn = "data/TestWidget-invalid.json";
  LOG(LL_INFO, ("widget_create_from_file(%s)", fn));
  w = widget_create_from_file(fn);
  ASSERT(!w, "invalid widget created");
  return 0;
}

int test_widget() {
  test_widget_create_from_file();
  return 0;
}
