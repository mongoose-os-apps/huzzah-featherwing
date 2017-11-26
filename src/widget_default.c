#include "mgos.h"
#include "tft.h"
#include "mongoose-touch.h"

static void widget_default_draw(struct widget_t *w) {
  if (!w)
    return;
  mgos_ili9341_setclipwin(w->x, w->y, w->x+w->w, w->y+w->h);

  mgos_ili9341_drawRect(0, 0, w->w, w->h, ILI9341_GREEN);

  if (w->img)
    mgos_ili9341_jpg_image(0, 0, 1, w->img, NULL, 0);
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
      widget_default_draw(w);
      break;
    case EV_WIDGET_TIMER:
    case EV_WIDGET_TOUCH_UP:
    case EV_WIDGET_TOUCH_DOWN:
    case EV_WIDGET_DESTROY:
    default: // EV_WIDGET_NONE
      break;
  }
}
