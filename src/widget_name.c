#include "mgos.h"
#include "mgos_config.h"
#include "mgos_wifi.h"
#include "mgos_net.h"
#include "mongoose-touch.h"

#define WIDGET_NAME_NAME    0
#define WIDGET_NAME_IPADDR  1
#define WIDGET_NAME_SSID    2
#define WIDGET_NAME_SCREEN  3
#define WIDGET_NAME_EMPTY   4

static uint8_t what = WIDGET_NAME_NAME;
extern struct screen_t *s_screen;
extern GFXfont FreeMonoBold9pt7b;

static void widget_name_render(struct widget_t *w, void *ev_data) {
  char namestring[21];
  char *p = NULL;
  int16_t text_height;


  if (!w)
    return;
  switch (what) {
    case WIDGET_NAME_NAME:
      sprintf(namestring, "%-20s",mgos_sys_config_get_app_hostname());
      break;
    case WIDGET_NAME_IPADDR: {
      char ip[16];
      struct mgos_net_event_data evd = {
        .if_type = MGOS_NET_IF_TYPE_WIFI, .if_instance = 0,
      };

      mgos_net_get_ip_info(MGOS_NET_IF_TYPE_WIFI, 0, &evd.ip_info);
      memset(ip, 0, sizeof(ip));
      mgos_net_ip_to_str(&evd.ip_info.ip, ip);
      sprintf(namestring, "%-20s",ip);
      break;
    }
    case WIDGET_NAME_SSID:
      p = mgos_wifi_get_connected_ssid();
      sprintf(namestring, "%-20s", p?p:"(no SSID)");
      free(p);
      break;
    case WIDGET_NAME_SCREEN:
      if (s_screen)
        sprintf(namestring, "%-20s", s_screen->name);
      else
        sprintf(namestring, "%-20s", "(no Screen)");
      break;
    case WIDGET_NAME_EMPTY:
        sprintf(namestring, "%-20s", "");
      break;
    default:
      sprintf(namestring, "%-20s",mgos_sys_config_get_app_hostname());
  }

  mgos_ili9341_set_window(w->x, w->y, w->x+w->w-1, w->y+w->h-1);
  mgos_ili9341_set_fgcolor565(ILI9341_BLACK);
  mgos_ili9341_fillRect(0, 0, w->w, w->h);

  mgos_ili9341_set_font(&FreeMonoBold9pt7b);
  mgos_ili9341_set_fgcolor565(ILI9341_GREEN);
  text_height=mgos_ili9341_getStringHeight(namestring);
  mgos_ili9341_print(2, text_height>w->h?0:(w->h-text_height)/2, namestring);

  (void) ev_data;
}

void widget_name_ev(int ev, struct widget_t *w, void *ev_data) {
  if (!w)
    return;

  switch(ev) {
    case EV_WIDGET_CREATE:
      break;
    case EV_WIDGET_DRAW:
    case EV_WIDGET_REDRAW:
    case EV_WIDGET_TIMER:
      widget_name_render(w, ev_data);
      break;
    case EV_WIDGET_TOUCH_UP:
      what++;
      if (what > WIDGET_NAME_EMPTY) what=WIDGET_NAME_NAME;
      widget_name_render(w, ev_data);
      break;
    case EV_WIDGET_TOUCH_DOWN:
    case EV_WIDGET_DESTROY:
    default: // EV_WIDGET_NONE
      break;
  }
}
