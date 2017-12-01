#include "mgos.h"
#include "mongoose-touch.h"
#include <esp_wifi.h>

static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Returns a value between 0 and 100%
static uint8_t widget_wifi_signal() {
  wifi_ap_record_t info;

  if(0 != esp_wifi_sta_get_ap_info(&info))
    return 0;

  if(info.rssi <= -100)
    return 0;
  if (info.rssi >= -50)
    return 100;
  return 2 * (info.rssi + 100);
}

static void widget_wifi_render(struct widget_t *w, void *ev_data) {
  uint8_t x, signal;

  mgos_ili9341_set_window(w->x, w->y, w->x+w->w, w->y+w->h);

  mgos_ili9341_set_fgcolor565(ILI9341_DARKGREY);
  mgos_ili9341_fillTriangle(2, 18, 18, 18, 18, 2);

  // Map signal strength from [0..100] to [0..16]
  signal = widget_wifi_signal();
  x = map(signal, 0, 100, 0, 16);
  if (x>0) {
    mgos_ili9341_set_fgcolor565(ILI9341_WHITE);
    mgos_ili9341_fillTriangle(2, 18, 2+x, 18, 2+x, 18-x);
  }

  // Draw an X in the corner if we don't have an IP address.
  if (mgos_wifi_get_status() != MGOS_WIFI_IP_ACQUIRED) {
    mgos_ili9341_set_fgcolor565(ILI9341_RED);
    mgos_ili9341_drawLine(11, 11, 16, 16);
    mgos_ili9341_drawLine(11, 16, 16, 11);
  }

  (void) ev_data;
}

void widget_wifi_ev(int ev, struct widget_t *w, void *ev_data) {
  if (!w)
    return;

  switch(ev) {
    case EV_WIDGET_CREATE:
    case EV_WIDGET_DRAW:
    case EV_WIDGET_REDRAW:
    case EV_WIDGET_TIMER:
      widget_wifi_render(w, ev_data);
      break;
    case EV_WIDGET_TOUCH_UP:
    case EV_WIDGET_TOUCH_DOWN:
    case EV_WIDGET_DESTROY:
    default: // EV_WIDGET_NONE
      break;
  }
}
