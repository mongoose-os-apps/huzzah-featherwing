#include "test.h"
#include "screen.h"
#include "widget.h"

extern int _mgos_timers;


static void test_screen_widget_add_and_remove(struct screen_t *s, const char *fn) {
  struct widget_t *w1;
  int num_widgets_before, num_widgets;
  bool ret;

  w1 = widget_create_from_file(fn);
  ASSERT(w1, "widget_create_from_file() failed");

  num_widgets_before = screen_get_num_widgets(s);
  LOG(LL_INFO, ("Screen has %d widget(s)", num_widgets_before));

  ret = screen_widget_add(s, w1);
  ASSERT(true == ret, "screen_widget_add() failed for widget1");
  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("Screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == num_widgets_before+1, "Did not see widget in screen");

  ret = screen_widget_destroy(s, &w1);
  ASSERT(true == ret, "screen_widget_destroy() failed for widget1");
  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("Screen has %d widget(s)", num_widgets));
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
  s = screen_create_from_file("data/TestScreen-invalid.json");
  ASSERT(!s, "created screen from invalid date");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("screen_create_from_file(data/TestScreen.json)"));
  s = screen_create_from_file("data/TestScreen.json");
  ASSERT(s, "Could not create screen");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("Screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == 2, "Expecting 2 widgets in screen");
  
  LOG(LL_INFO, ("test_screen_widget_add_and_remove(data/TestWidget.json)"));
  test_screen_widget_add_and_remove(s, "data/TestWidget.json");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("Screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == 2, "Expecting 2 widgets in screen");
  
  LOG(LL_INFO, ("test_widget_add_from_file(data/TestWidget.json)"));
  w = screen_widget_add_from_file(s, "data/TestWidget.json");
  ASSERT(w, "screen_widget_add_from_file() failed for data/TestWidget.json");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("Screen has %d widget(s)", num_widgets));
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
