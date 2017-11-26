#include "mgos.h"
#include "tft.h"
#include "mongoose-touch.h"

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
    case EV_WIDGET_TOUCH_DOWN:
    case EV_WIDGET_DESTROY:
    default: // EV_WIDGET_NONE
      break;
  }
}
