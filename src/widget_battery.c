#include "mgos.h"
#include "mgos_adc.h"
#include "mgos_config.h"
#include "tft.h"
#include "mongoose-touch.h"
#include "mgos_prometheus_metrics.h"


#define WIDGET_BATTERY_ADC_PIN 35

// Returns millivolts - Huzzah32 has a 1:1 voltage divider on I35 (A13)
// so it returns half of the battery voltage. Calibrate the function by
// measuring the ADC output at 4000mV.
static int widget_battery_getvoltage() {
  int val, mvolts;

  val = mgos_adc_read(WIDGET_BATTERY_ADC_PIN);
  mvolts=(4000*val)/mgos_sys_config_get_app_battery_calibration();
  LOG(LL_DEBUG, ("ADC value %d, voltage %d mV", val, mvolts));
  return mvolts;
}

static void widget_battery_render(struct widget_t *w, void *ev_data) {
  int mvolts;
  color_t color;
  uint8_t h;

  if (!w)
    return;

  mgos_ili9341_setclipwin(w->x, w->y, w->x+w->w, w->y+w->h);

  mvolts=widget_battery_getvoltage();
  color=ILI9341_RED;
  if (mvolts<3200) mvolts=3200;
  if (mvolts>3500) color=ILI9341_YELLOW;
  if (mvolts>3900) color=ILI9341_GREEN;
  if (mvolts>4200) mvolts=4200;
  h=12-12*(mvolts-3200)/(4200-3200);

  mgos_ili9341_fillRect(5, 2, 3, 2, color);
  mgos_ili9341_fillRect(2, 4, 9, 14, color);
  mgos_ili9341_fillRect(3, 5, 7, h, ILI9341_BLACK);
  mgos_ili9341_resetclipwin();

  (void) ev_data;
}

static void prometheus_battery_metrics_fn(struct mg_connection *nc, void *user_data) {
  mgos_prometheus_metrics_printf(nc, GAUGE,
    "battery_mvolts", "Battery voltage in mV",
    "%d", widget_battery_getvoltage());

  (void) user_data;
}


void widget_battery_ev(int ev, struct widget_t *w, void *ev_data) {
  if (!w)
    return;

  switch(ev) {
    case EV_WIDGET_CREATE:
      LOG(LL_INFO, ("Monitoring LiPo voltage on gpio=%d", WIDGET_BATTERY_ADC_PIN));
      mgos_adc_enable(WIDGET_BATTERY_ADC_PIN);
      mgos_prometheus_metrics_add_handler(prometheus_battery_metrics_fn, NULL);
    case EV_WIDGET_DRAW:
    case EV_WIDGET_REDRAW:
    case EV_WIDGET_TIMER:
      widget_battery_render(w, ev_data);
      break;
    case EV_WIDGET_TOUCH_UP:
    case EV_WIDGET_TOUCH_DOWN:
    case EV_WIDGET_DESTROY:
    default: // EV_WIDGET_NONE
      break;
  }
}
