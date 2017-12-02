#include "mgos.h"
#include "mongoose-touch.h"

static void widget_topbar_render(struct widget_t *w, void *ev_data) {
  if (!w)
    return;

  mgos_ili9341_set_window(w->x, w->y, w->x+w->w-1, w->y+w->h-1);
  mgos_ili9341_set_fgcolor565(ILI9341_WHITE);
  mgos_ili9341_fillRect(0, 0, w->w, w->h);

  (void) ev_data;
}

void widget_topbar_ev(int ev, struct widget_t *w, void *ev_data) {
  if (!w)
    return;

  switch(ev) {
    case EV_WIDGET_CREATE:
      break;
    case EV_WIDGET_DRAW:
    case EV_WIDGET_REDRAW:
    case EV_WIDGET_TIMER:
      widget_topbar_render(w, ev_data);
      break;
    case EV_WIDGET_TOUCH_UP:
    case EV_WIDGET_TOUCH_DOWN:
    case EV_WIDGET_DESTROY:
    default: // EV_WIDGET_NONE
      break;
  }
}
