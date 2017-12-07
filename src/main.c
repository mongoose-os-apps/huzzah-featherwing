#include <stdio.h>
#include <time.h>

#include "mgos.h"
#include "mgos_pwm.h"
#include "mongoose-touch.h"
#include "fonts/FreeSerifBold9pt7b.h"
#include "fonts/FreeMonoBold9pt7b.h"

struct screen_t *s_screen = NULL;

static void touch_handler(struct mgos_stmpe610_event_data *ed) {
  struct widget_t *widget;

  if (!ed) return;
  LOG(LL_INFO, ("Touch %s at (%d,%d) pressure=%d, length=%d", ed->direction==TOUCH_UP?"UP":"DOWN", ed->x, ed->y, ed->z, ed->length));

  // If the backlight is inactive, grab the TOUCH_DOWN and TOUCH_UP events.
  // Do nothing on TOUCH_DOWN, but reactivate the backlight by calling
  // backlight_keepalive() upon TOUCH_UP. This way, the first touch will
  // wake up the UI, but not send events to widgets and things.
  if (!backlight_active()) {
    if (ed->direction==TOUCH_DOWN) {
      LOG(LL_INFO, ("Ignoring touch event, backlight is inactive"));
      return;
    }
    LOG(LL_INFO, ("Ignoring touch event, but setting backlight on"));
    backlight_keepalive();
    return;
  }

  backlight_keepalive();

  widget = screen_widget_find_by_xy(s_screen, ed->x, ed->y);

  if (ed->direction==TOUCH_DOWN) {
    widget_network_recv();
    if (widget && widget->handler)
      widget->handler(EV_WIDGET_TOUCH_DOWN, widget, ed);
  } else {
    widget_network_send();
    if (widget && widget->handler)
      widget->handler(EV_WIDGET_TOUCH_UP, widget, ed);
  }
}

void tft_demo(void)
{
  mgos_ili9341_set_rotation(mgos_sys_config_get_tft_orientation());
  mgos_ili9341_set_dimensions(320, 240);
  mgos_stmpe610_set_handler(touch_handler);
  mgos_stmpe610_set_dimensions(320, 240);

  mgos_ili9341_set_fgcolor(0,0,0);
  mgos_ili9341_fillScreen();

  s_screen = screen_create_from_file("/screen_main.json", widget_default_ev, NULL);
  if (!s_screen) {
    LOG(LL_ERROR, ("Could not load screen"));
    return;
  }
  screen_add_default_widgets(s_screen);
  screen_widget_broadcast(s_screen, EV_WIDGET_DRAW, NULL);
  LOG(LL_INFO, ("Screen '%s' has %d widgets", s_screen->name, screen_get_num_widgets(s_screen)));
}

enum mgos_app_init_result mgos_app_init(void)
{
  backlight_init();

  tft_demo();

  return MGOS_APP_INIT_SUCCESS;
}
