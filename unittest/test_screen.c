#include "test.h"
#include "screen.h"
#include "widget.h"

extern int _mgos_timers;

static void test_screen_default_ev(int ev, struct widget_t *w, void *ev_data) {
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

static void test_screen_widget_add_and_remove(struct screen_t *s, const char *fn) {
  struct widget_t *w1;
  int num_widgets_before, num_widgets;
  bool ret;
  int mgos_timers_before;

  LOG(LL_INFO, ("widget_create_from_file(%s)", fn));
  w1 = widget_create_from_file(fn);
  ASSERT(w1, "widget_create_from_file() failed");

  LOG(LL_INFO, ("widget_set_timer()"));
  mgos_timers_before=_mgos_timers;
  widget_set_timer(w1, 1000);
  ASSERT(_mgos_timers==mgos_timers_before+1, "timer not found");

  num_widgets_before = screen_get_num_widgets(s);
  LOG(LL_INFO, ("screen has %d widget(s)", num_widgets_before));

  LOG(LL_INFO, ("screen_widget_add() of widget1"));
  ret = screen_widget_add(s, w1);
  ASSERT(true == ret, "screen_widget_add() failed for widget1");
  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == num_widgets_before+1, "Did not see widget in screen");

  LOG(LL_INFO, ("screen_widget_destroy() of widget1"));
  ret = screen_widget_destroy(s, &w1);
  ASSERT(true == ret, "screen_widget_destroy() failed for widget1");
  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == num_widgets_before, "Too many widgets in screen");

  return;
}

static void test_screen_widget_find(struct screen_t *s) {
  struct widget_t *w;
  uint16_t x, y;

  x=16; y=16;
  LOG(LL_INFO, ("want to find widget at (%d,%d)", x, y));
  w = screen_widget_find_by_xy(s, x, y);
  ASSERT(w, "widget not found");

  x=256; y=16;
  LOG(LL_INFO, ("want to find widget at (%d,%d)", x, y));
  w = screen_widget_find_by_xy(s, x, y);
  ASSERT(w, "widget not found");

  x=256+47; y=16+47;
  LOG(LL_INFO, ("want to find widget at (%d,%d)", x, y));
  w = screen_widget_find_by_xy(s, x, y);
  ASSERT(w, "widget not found");

  x=16+47; y=16+47;
  LOG(LL_INFO, ("want to find widget at (%d,%d)", x, y));
  w = screen_widget_find_by_xy(s, x, y);
  ASSERT(w, "widget not found");

  x=15; y=16;
  LOG(LL_INFO, ("do not want to find widget at (%d,%d)", x, y));
  w = screen_widget_find_by_xy(s, x, y);
  ASSERT(!w, "widget found");

  x=256+48; y=16+48;
  LOG(LL_INFO, ("do not want to find widget at (%d,%d)", x, y));
  w = screen_widget_find_by_xy(s, x, y);
  ASSERT(!w, "widget found");


  (void) s;
}

int test_screen() {
  struct screen_t *s = NULL;
  struct widget_t *w = NULL;
  uint16_t num_widgets;

  LOG(LL_INFO, ("screen_create_from_file(data/TestScreen-invalid.json)"));
  s = screen_create_from_file("data/TestScreen-invalid.json", test_screen_default_ev, NULL);
  ASSERT(!s, "created screen from invalid date");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("screen_create_from_file(data/TestScreen.json)"));
  s = screen_create_from_file("data/TestScreen.json", test_screen_default_ev, NULL);
  ASSERT(s, "Could not create screen");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == 2, "Expecting 2 widgets in screen");
  
  LOG(LL_INFO, ("test_screen_widget_add_and_remove(data/TestWidget.json)"));
  test_screen_widget_add_and_remove(s, "data/TestWidget.json");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == 2, "Expecting 2 widgets in screen");
  
  LOG(LL_INFO, ("test_widget_add_from_file(data/TestWidget.json)"));
  w = screen_widget_add_from_file(s, "data/TestWidget.json");
  ASSERT(w, "screen_widget_add_from_file() failed for data/TestWidget.json");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == 3, "Expecting 3 widgets in screen");
  
  LOG(LL_INFO, ("screen_widget_destroy()"));
  screen_widget_destroy(s, &w);
  ASSERT(!w, "screen_widget_destroy() failed for data/TestWidget.json");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("Screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == 2, "Expecting 2 widgets in screen");

  LOG(LL_INFO, ("test_screen_widget_find()"));
  test_screen_widget_find(s);
  
  LOG(LL_INFO, ("screen_destroy()"));
  screen_destroy(&s);
  ASSERT(!s, "Could not destroy screen");

  LOG(LL_INFO, ("ensure we have no timers"));
  ASSERT(_mgos_timers==0, "timers found");

  return 0;
}
