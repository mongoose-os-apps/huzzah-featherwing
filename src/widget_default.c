#include "mgos.h"
#include "mongoose-touch.h"

extern GFXfont FreeSerifBold9pt7b;

static void widget_default_destroy(struct widget_t *w) {
  if (!w)
    return;
  mgos_ili9341_set_window(w->x, w->y, w->x+w->w-1, w->y+w->h-1);
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
      widget_default_destroy(w);
      break;
    default: // EV_WIDGET_NONE
      break;
  }
  (void) ev_data;
}
