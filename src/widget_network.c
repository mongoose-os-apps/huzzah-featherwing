#include "mgos.h"
#include "mgos_timers.h"
#include "tft.h"
#include "mongoose-touch.h"

static mgos_timer_id send_timer = 0;
static mgos_timer_id recv_timer = 0;
static struct widget_t *widget_network = NULL;

static void widget_network_render(struct widget_t *w, void *ev_data) {
  mgos_ili9341_setclipwin(w->x, w->y, w->x+w->w, w->y+w->h);
  // Send
  mgos_ili9341_fillTriangle(2, 10, 10, 10, 6, 2, send_timer?ILI9341_YELLOW:ILI9341_DARKGREY);
  mgos_ili9341_fillRect(5, 10, 3, 8, send_timer?ILI9341_YELLOW:ILI9341_DARKGREY);

  // Recv
  mgos_ili9341_fillTriangle(12, 10, 20, 10, 16, 18, recv_timer?ILI9341_YELLOW:ILI9341_DARKGREY);
  mgos_ili9341_fillRect(15, 2, 3, 8, recv_timer?ILI9341_YELLOW:ILI9341_DARKGREY);
  mgos_ili9341_resetclipwin();

  (void) ev_data;
}

static void widget_network_send_clear(void *arg) {
  send_timer=0;
  widget_network_ev(EV_WIDGET_REDRAW, widget_network, NULL);
}

static void widget_network_recv_clear(void *arg) {
  recv_timer=0;
  widget_network_ev(EV_WIDGET_REDRAW, widget_network, NULL);
}

void widget_network_send() {
  if (!widget_network)
    return;

  if (!send_timer)
    send_timer=mgos_set_timer(100, 0, widget_network_send_clear, NULL);
  widget_network_ev(EV_WIDGET_REDRAW, widget_network, NULL);
}

void widget_network_recv() {
  if (!widget_network)
    return;

  if (!recv_timer)
    recv_timer=mgos_set_timer(100, 0, widget_network_recv_clear, NULL);
  widget_network_ev(EV_WIDGET_REDRAW, widget_network, NULL);
}

void widget_network_ev(int ev, struct widget_t *w, void *ev_data) {
  if (!w)
    return;

  switch(ev) {
    case EV_WIDGET_CREATE:
      widget_network=w;
    case EV_WIDGET_DRAW:
    case EV_WIDGET_REDRAW:
      widget_network_render(w, ev_data);
      break;
    case EV_WIDGET_DESTROY:
      widget_network=NULL;
      send_timer=0;
      recv_timer=0;
      break;
    case EV_WIDGET_TIMER:
    case EV_WIDGET_TOUCH_UP:
    case EV_WIDGET_TOUCH_DOWN:
    default: // EV_WIDGET_NONE
      break;
  }
}
