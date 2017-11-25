#include "mgos.h"
#include "tft.h"
#include "widget.h"

static void widget_time_render(struct widget_t *w, void *ev_data) {
  char tmp_buff[32];

  time_t now = 3600 + time(0); // TZ=GMT+1
  struct tm* tm_info = gmtime(&now);

  if (!w)
    return;

  mgos_ili9341_set_fg(&ILI9341_YELLOW);
  sprintf(tmp_buff, " %02d:%02d:%02d", tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
  mgos_ili9341_setclipwin(w->x, w->y, w->x+w->w, w->y+w->h);
  mgos_ili9341_print(tmp_buff, 2, 4);
  mgos_ili9341_resetclipwin();

  (void) ev_data;
}

void widget_time_ev(int ev, struct widget_t *w, void *ev_data) {
  if (!w)
    return;

  switch(ev) {
    case EV_WIDGET_CREATE:
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
