#include <stdio.h>
#include <time.h>

#include "mgos.h"
#include "tft.h"
#include "stmpe610.h"
#include "mongoose-touch.h"

static long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void touch_handler(struct mgos_stmpe610_event_data *ed) {
  uint16_t x, y;
  struct widget_t *widget;

  if (!ed) return;

  x = map(ed->x, 0, 4095, 0, _width-1);
  y = map(ed->y, 0, 4095, 0, _height-1);
  LOG(LL_INFO, ("Touch %s at (%d,%d) pressure=%d, length=%d", ed->direction==TOUCH_UP?"UP":"DOWN", x, y, ed->z, ed->length));
  widget = widget_find(x, y);

  if (ed->direction==TOUCH_DOWN) {
//    mgos_ili9341_drawCircle(x, y, ed->length, ILI9341_YELLOW);
    widget_network_recv();
    if (widget)
      widget->handler(EV_WIDGET_TOUCH_DOWN, widget, NULL);
  } else {
//    mgos_ili9341_drawCircle(x, y, ed->length, ILI9341_BLUE);
    widget_network_send();
    if (widget)
      widget->handler(EV_WIDGET_TOUCH_UP, widget, NULL);
  }
}

void tft_demo(void)
{
  mgos_ili9341_setRotation(mgos_sys_config_get_tft_orientation());
  mgos_stmpe610_set_rotation(mgos_sys_config_get_tft_orientation());
  mgos_ili9341_setGammaCurve(DEFAULT_GAMMA_CURVE);
  mgos_ili9341_setFont(DEFAULT_FONT, NULL);

  mgos_ili9341_jpg_image(CENTER, CENTER, 1, "mongoose-os.jpg", NULL, 0);
//  mgos_ili9341_jpg_image(200, 150, 2, "flower.jpg", NULL, 0);

  widget_add(0, 0, 198, 20, 0, widget_name_ev, NULL);
  widget_add(198, 0, 22, 20, 0, widget_network_ev, NULL);
  widget_add(220, 0, 20, 20, 5000, widget_wifi_ev, NULL);
  widget_add(240, 0, 80, 20, 1000, widget_time_ev, NULL);
  widget_add(0, 21, 320, 2, 0, widget_topbar_ev, NULL);
}

enum mgos_app_init_result mgos_app_init(void)
{
  mgos_stmpe610_set_handler(touch_handler);

  tft_demo();

  return MGOS_APP_INIT_SUCCESS;
}
