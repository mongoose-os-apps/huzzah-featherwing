#include "test.h"
#include "widget.h"

extern int _mgos_timers;

static void test_widget_default_ev(int ev, struct widget_t *w, void *ev_data) {
  char evname[15];

  if (!w)
    return;

  widget_ev_to_str(ev, evname, sizeof(evname)-1);

  LOG(LL_INFO, ("Event %s received for widget '%s'", evname, w->name));

  switch(ev) {
    case EV_WIDGET_CREATE:
    case EV_WIDGET_DRAW:
    case EV_WIDGET_REDRAW:
    case EV_WIDGET_TIMER:
    case EV_WIDGET_TOUCH_UP:
    case EV_WIDGET_TOUCH_DOWN:
    case EV_WIDGET_DESTROY:
    default: // EV_WIDGET_NONE
      break;
  }
  (void) ev_data;
}

static int test_widget_create_from_file(void) {
  struct widget_t *w;
  int ret;

  char *fn = "data/TestWidget.json";
  LOG(LL_INFO, ("widget_create_from_file(%s)", fn));
  w = widget_create_from_file(fn);
  ASSERT(w, "widget_create_from_file()");
  ASSERT(w->x == 16, "'x' field is invalid");
  ASSERT(w->y == 16, "'x' field is invalid");
  ASSERT(w->w == 48, "'x' field is invalid");
  ASSERT(w->h == 48, "'x' field is invalid");
  ret = strncmp("/some/file.ext", w->img, strlen("/some/file.ext"));
  ASSERT(ret == 0, "'img' field is invalid");
  ret = strncmp("One", w->label, strlen("One"));
  ASSERT(ret == 0, "'label' field is invalid");

  LOG(LL_INFO, ("widget_set_timer()"));
  widget_set_timer(w, 1000);
  ASSERT(_mgos_timers==1, "timer not found");

  LOG(LL_INFO, ("widget_set_handler()"));
  widget_set_handler(w, test_widget_default_ev, NULL);
  

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
