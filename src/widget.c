#include "mgos.h"
#include "widget.h"
#include "frozen/frozen.h"
#include "common/queue.h"

struct widget_list_t {
  struct widget_t *widget;
  SLIST_ENTRY(widget_list_t) entries;
};

SLIST_HEAD(widget_list, widget_list_t) s_widgets;

static void widget_event_timer(void *arg) {
  struct widget_t *widget = (struct widget_t *) arg;
  if (!widget)
    return;
  if (widget->handler)
    widget->handler(EV_WIDGET_TIMER, widget, NULL);
}

static void widget_destroy(struct widget_t *widget) {
  if (!widget)
    return;
  if (widget->handler)
    widget->handler(EV_WIDGET_DESTROY, widget, NULL);

  if (widget->timer_msec)
    mgos_clear_timer(widget->_timer_id);
  if (widget->user_data)
    free(widget->user_data);
  free(widget);
}

struct widget_t *widget_add(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t timer_msec, widget_event_fn handler, void *user_data) {
  struct widget_t *widget;
  struct widget_list_t *wl;

  widget = (struct widget_t *) calloc(1, sizeof(*widget));
  if (!widget)
    return NULL;
  widget->x=x; 
  widget->y=y; 
  widget->w=w; 
  widget->h=h; 
  widget->timer_msec=timer_msec; 
  widget->handler=handler;
  widget->user_data=user_data;
  if (timer_msec > 0)
    widget->_timer_id = mgos_set_timer(timer_msec, MGOS_TIMER_REPEAT, widget_event_timer, widget);

  wl = (struct widget_list_t *) calloc(1, sizeof(*wl));
  if (!wl) {
    widget_destroy(widget);
    return NULL;
  }
  wl->widget = widget;
  SLIST_INSERT_HEAD(&s_widgets, wl, entries);

  if (handler)
    handler(EV_WIDGET_CREATE, widget, NULL);
  return widget;
}

struct widget_t *widget_find(uint16_t x, uint16_t y) {
  struct widget_list_t *wl;

  SLIST_FOREACH(wl, &s_widgets, entries) {
    if (wl->widget->x < x &&
        x < (wl->widget->x+wl->widget->w) &&
        wl->widget->y < y &&
        y < (wl->widget->y+wl->widget->h))
      return wl->widget;
  }
  return NULL;
}

void widget_remove(struct widget_t *widget) {
  struct widget_list_t *wl;
  if (!widget)
    return;

  SLIST_FOREACH(wl, &s_widgets, entries) {
    if (wl->widget == widget) {
      SLIST_REMOVE(&s_widgets, wl, widget_list_t, entries);
      widget_destroy(widget);
    }
  }
}

struct widget_t *widget_add_from_file(const char *fn, uint32_t timer_msec, widget_event_fn handler, void *user_data) {
  char *json;
  int x, y, w, h;
  int type = 0;
  char *label = NULL;
  char *icon = NULL;
  struct widget_t *widget=NULL;

  json = json_fread(fn);
  if (!json) {
    LOG(LL_ERROR, ("%s: Could not json_fread()", fn));
    widget=NULL; goto exit;
  }
  if (json_scanf(json, strlen(json), "{x:%d,y:%d,w:%d,h:%d}", &x, &y, &w, &h) != 4) {
    LOG(LL_ERROR, ("%s: Incomplete JSON: require 'x', 'y', 'w', 'h' fields", fn));
    widget=NULL; goto exit;
  }
  json_scanf(json, strlen(json), "{type:%d,label:%Q,icon:%Q}", &type, &label, &icon);
  free(json);

  LOG(LL_INFO, ("%s: x=%d y=%d w=%d h=%d type=%d label='%s' icon='%s'", fn, x, y, w, h, type, label?label:"(empty)",icon?icon:"(empty)"));
  widget = widget_add(x, y, w, h, 0, NULL, NULL);

exit:
  if (label) free(label);
  if (icon) free(icon);
  return widget;

  (void) timer_msec;
  (void) handler;
  (void) user_data;
}
