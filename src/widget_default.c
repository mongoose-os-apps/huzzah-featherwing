#include "mgos.h"
#include "mgos_mqtt.h"
#include "mongoose-touch.h"

extern GFXfont FreeSerifBold9pt7b;
extern struct screen_t *s_screen;

static void widget_default_destroy(struct widget_t *w) {
  if (!w)
    return;
  mgos_ili9341_set_window(w->x, w->y, w->x+w->w, w->y+w->h);
  mgos_ili9341_set_fgcolor565(ILI9341_BLACK);
  mgos_ili9341_fillRect(0,0,w->w,w->h);
}

static void widget_default_draw(struct widget_t *w, uint16_t color) {
  if (!w)
    return;
  mgos_ili9341_set_window(w->x, w->y, w->x+w->w-1, w->y+w->h-1);

  mgos_ili9341_set_fgcolor565(color);
 
  if (w->img) {
    mgos_ili9341_set_fgcolor565(ILI9341_BLUE);
    mgos_ili9341_drawDIF(0, 0, w->img);
  } else if (w->label) {
    int16_t text_width, text_height;
    uint16_t x, y;

    mgos_ili9341_drawRoundRect(0, 0, w->w, w->h, 8);
    mgos_ili9341_set_font(&FreeSerifBold9pt7b);
    text_width=mgos_ili9341_getStringWidth(w->label);
    text_height=mgos_ili9341_getStringHeight(w->label);
    if (text_width>w->w)
      x=0;
    else
      x=(w->w-text_width)/2;
    if (text_height>w->h)
      y=0;
    else
      y=(w->h-text_height)/2;
    mgos_ili9341_print(x, y, w->label);
  }
}

void screen_add_default_widgets(struct screen_t *screen) {
  struct widget_t *w;
  uint16_t screen_width = mgos_ili9341_get_screenWidth();
  uint16_t screen_height = mgos_ili9341_get_screenHeight();
  if (!screen)
    return;

  w = widget_create("topbar", 0, 21, screen_width, 2);
  widget_set_handler(w, widget_topbar_ev);
  screen_widget_add(screen, w);

  w = widget_create("time", screen_width-80, 0, 80, 20);
  widget_set_handler(w, widget_time_ev);
  widget_set_timer(w, 1000);
  screen_widget_add(screen, w);
  screen_width-=80;

  w = widget_create("battery", screen_width-13, 0, 13, 20);
  widget_set_handler(w, widget_battery_ev);
  widget_set_timer(w, 10000);
  screen_widget_add(screen, w);
  screen_width-=13;

  w = widget_create("network", screen_width-22, 0, 22, 20);
  widget_set_handler(w, widget_network_ev);
  screen_widget_add(screen, w);
  screen_width-=22;

  w = widget_create("wifi", screen_width-20, 0, 20, 20);
  widget_set_handler(w, widget_wifi_ev);
  widget_set_timer(w, 5000);
  screen_widget_add(screen, w);
  screen_width-=20;

  w = widget_create("name", 0, 0, screen_width, 20);
  widget_set_handler(w, widget_name_ev);
  screen_widget_add(screen, w);
}


static void widget_default_loadscreen(struct widget_t *w, void *ev_data) {
  struct screen_t *new_screen;
  char *new_screen_fn;

  if (!w)
    return;
  if (!w->user_data) {
    LOG(LL_ERROR, ("Widget '%s' does not have user_data set", w->name));
    return;
  }

  new_screen_fn=strdup(w->user_data);

  screen_destroy(&s_screen);
  new_screen = screen_create_from_file(new_screen_fn, widget_default_ev, NULL);
  free(new_screen_fn);

  if (!new_screen) {
    LOG(LL_ERROR, ("Could not load screen"));
    return;
  }
  screen_add_default_widgets(new_screen);

  LOG(LL_INFO, ("Navigating to new screen '%s' which has %d widgets", new_screen->name, screen_get_num_widgets(new_screen)));
  s_screen = new_screen;
  screen_widget_broadcast(s_screen, EV_WIDGET_DRAW, NULL);

  (void) ev_data;
}


static void widget_default_mqtt_send(struct widget_t *w, void *ev_data) {
  struct json_token val;
  int idx=0;

  if (!w)
    return;
  if (!w->user_data)
    return;
//  LOG(LL_DEBUG, ("MQTT string: '%s'", (char *)w->user_data));
  // Traverse Array
  for (idx = 0; json_scanf_array_elem(w->user_data, strlen(w->user_data), "", idx, &val) > 0; idx++) {
    char *t=NULL, *m=NULL;
    uint16_t t_len=0, m_len=0;
    char *topic;

    LOG(LL_DEBUG, ("Index %d, token [%.*s]", idx, val.len, val.ptr));
    t=(char*)val.ptr;
    m=strstr(val.ptr, " ");
    if (m-val.ptr <= val.len) {
      t_len=m-t;
      m++;
      m_len=val.len-t_len-1;
    } else {
      t_len=val.len;
      m_len=0;
      m=NULL;
    }
    if ((topic=malloc(t_len+1))) {
      memcpy(topic, t, t_len);
      topic[t_len]=0;
      LOG(LL_INFO, ("Sending topic='%s', message='%.*s'", topic, m_len, m));
      mgos_mqtt_pub(topic, m, m_len, 0, false);
      free(topic);
      widget_network_send();
    }
  }
  (void) ev_data;
}

void widget_default_ev(int ev, struct widget_t *w, void *ev_data) {
  char evname[15];

  if (!w)
    return;
  widget_ev_to_str(ev, evname, sizeof(evname)-1);

  LOG(LL_DEBUG, ("Event %s received for widget '%s'", evname, w->name));

  switch(ev) {
    case EV_WIDGET_CREATE:
      break;
    case EV_WIDGET_DRAW:
    case EV_WIDGET_REDRAW:
    case EV_WIDGET_TIMER:
      widget_default_draw(w, ILI9341_GREEN);
      break;
    case EV_WIDGET_TOUCH_UP:
      if (w->type == WIDGET_TYPE_LOADSCREEN) {
        widget_default_loadscreen(w, ev_data);
        break;
      }
      if (w->type == WIDGET_TYPE_MQTT_BUTTON) {
        widget_default_mqtt_send(w, ev_data);
      }
      widget_default_draw(w, ILI9341_GREEN);
      break;
    case EV_WIDGET_TOUCH_DOWN:
      widget_default_draw(w, ILI9341_RED);
      break;
    case EV_WIDGET_DESTROY:
      widget_default_destroy(w);
      break;
    default: // EV_WIDGET_NONE
      break;
  }
  (void) ev_data;
}
