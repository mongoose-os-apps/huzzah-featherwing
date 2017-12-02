#include "mgos.h"
#include "mongoose-touch.h"

extern GFXfont FreeSerifBold9pt7b;

static void widget_time_render(struct widget_t *w, void *ev_data) {
  char tmp_buff[32];
  int16_t text_width, text_height;

  time_t now = 3600 + time(0); // TZ=GMT+1
  struct tm* tm_info = gmtime(&now);

  if (!w)
    return;

  mgos_ili9341_set_fgcolor565(ILI9341_YELLOW);
  sprintf(tmp_buff, " %02d:%02d:%02d", tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
  mgos_ili9341_set_window(w->x, w->y, w->x+w->w-1, w->y+w->h-1);
  mgos_ili9341_set_font(&FreeSerifBold9pt7b);

  text_width=mgos_ili9341_getStringWidth(tmp_buff);
  text_height=mgos_ili9341_getStringHeight(tmp_buff);

  mgos_ili9341_print(text_width>w->w?0:(w->w-text_width)/2, text_height>w->h?0:(w->h-text_height)/2, tmp_buff);

  (void) ev_data;
}

void widget_time_ev(int ev, struct widget_t *w, void *ev_data) {
  if (!w)
    return;

  switch(ev) {
    case EV_WIDGET_CREATE:
      break;
    case EV_WIDGET_DRAW:
    case EV_WIDGET_REDRAW:
    case EV_WIDGET_TIMER:
      widget_time_render(w, ev_data);
      break;
    case EV_WIDGET_TOUCH_UP:
    case EV_WIDGET_TOUCH_DOWN:
    case EV_WIDGET_DESTROY:
    default: // EV_WIDGET_NONE
      break;
  }
}
