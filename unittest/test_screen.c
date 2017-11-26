#include "test.h"
#include "screen.h"
#include "widget.h"


static void test_widget_add_and_remove(struct screen_t *s, const char *fn) {
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

  (void) s;
  (void) fn;
}


int test_screen() {
  struct screen_t *s = NULL;
  struct widget_t *w = NULL;
  uint16_t num_widgets;

  LOG(LL_INFO, ("screen_create_from_file(data/TestScreen.json)"));
  s = screen_create_from_file("data/TestScreen.json");
  ASSERT(s, "Could not create screen");

  num_widgets = screen_get_num_widgets(s);
  LOG(LL_INFO, ("Screen has %d widget(s)", num_widgets));
  ASSERT(num_widgets == 2, "Expecting 2 widgets in screen");
  
  LOG(LL_INFO, ("test_widget_add_and_remove(data/TestWidget.json)"));
  test_widget_add_and_remove(s, "data/TestWidget.json");

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
  

  LOG(LL_INFO, ("screen_destroy()"));
  screen_destroy(&s);
  ASSERT(!s, "Could not destroy screen");

  return 0;
}
