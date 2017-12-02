#ifndef __MONGOOSE_TOUCH_H
#define __MONGOOSE_TOUCH_H

#include "widget.h"
#include "screen.h"
#include "backlight.h"
#include "mgos_ili9341.h"
#include "mgos_stmpe610.h"

void widget_time_ev(int ev, struct widget_t *w, void *ev_data);

void widget_wifi_ev(int ev, struct widget_t *w, void *ev_data);

void widget_network_ev(int ev, struct widget_t *w, void *ev_data);
void widget_network_send();
void widget_network_recv();

void widget_topbar_ev(int ev, struct widget_t *w, void *ev_data);

void widget_name_ev(int ev, struct widget_t *w, void *ev_data);

void widget_battery_ev(int ev, struct widget_t *w, void *ev_data);

void widget_default_ev(int ev, struct widget_t *w, void *ev_data);

void screen_add_default_widgets(struct screen_t *screen);


#endif // __MONGOOSE_TOUCH_H
