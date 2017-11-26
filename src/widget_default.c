#include "mgos.h"
#include "tft.h"
#include "mongoose-touch.h"

static void widget_default_draw(struct widget_t *w, color_t color) {
  if (!w)
    return;
  mgos_ili9341_setclipwin(w->x, w->y, w->x+w->w, w->y+w->h);

  mgos_ili9341_drawRoundRect(0, 0, w->w, w->h, 8, color);

  if (w->img)
    mgos_ili9341_png(0, 0, w->img);
}

void widget_default_ev(int ev, struct widget_t *w, void *ev_data) {
  char evname[15];

  if (!w)
    return;
  widget_ev_to_str(ev, evname, sizeof(evname)-1);

  LOG(LL_INFO, ("Event %s received for widget '%s'", evname, w->name));

  switch(ev) {
    case EV_WIDGET_CREATE:
    case EV_WIDGET_DRAW:
    case EV_WIDGET_REDRAW:
    case EV_WIDGET_TIMER:
    case EV_WIDGET_TOUCH_UP:
      widget_default_draw(w, ILI9341_GREEN);
      break;
    case EV_WIDGET_TOUCH_DOWN:
      widget_default_draw(w, ILI9341_RED);
      break;
    case EV_WIDGET_DESTROY:
    default: // EV_WIDGET_NONE
      break;
  }
}
