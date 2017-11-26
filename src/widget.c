#include "mgos.h"
#include "widget.h"
#include "frozen/frozen.h"
#include "common/queue.h"

static void widget_event_timer(void *arg) {
  struct widget_t *widget = (struct widget_t *) arg;
  if (!widget)
    return;
  if (widget->handler)
    widget->handler(EV_WIDGET_TIMER, widget, NULL);
}

void widget_destroy(struct widget_t **widget) {
  if (!*widget)
    return;
  if ((*widget)->handler)
    (*widget)->handler(EV_WIDGET_DESTROY, *widget, NULL);

  if ((*widget)->timer_msec)
    mgos_clear_timer((*widget)->_timer_id);
  if ((*widget)->user_data)
    free((*widget)->user_data);
  free(*widget);
  *widget=NULL;
}

struct widget_t *widget_create(char *name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t timer_msec, widget_event_fn handler, void *user_data) {
  struct widget_t *widget;

  widget = (struct widget_t *) calloc(1, sizeof(*widget));
  if (!widget)
    return NULL;
  if (name)
    widget->name=strdup(name);
  widget->x=x; 
  widget->y=y; 
  widget->w=w; 
  widget->h=h; 
  widget->timer_msec=timer_msec; 
  widget->handler=handler;
  widget->user_data=user_data;
  if (timer_msec > 0)
    widget->_timer_id = mgos_set_timer(timer_msec, MGOS_TIMER_REPEAT, widget_event_timer, widget);

  if (handler)
    handler(EV_WIDGET_CREATE, widget, NULL);

  return widget;
}

struct widget_t *widget_create_from_json(const char *json) {
  struct widget_t *widget=NULL;
  int x, y, w, h;
  int type = 0;
  char *name = NULL;
  char *label = NULL;
  char *icon = NULL;

  if (json_scanf(json, strlen(json), "{name:%Q,x:%d,y:%d,w:%d,h:%d}", &name, &x, &y, &w, &h) != 5) {
    LOG(LL_ERROR, ("Incomplete JSON: require 'x', 'y', 'w', 'h' and 'name' fields"));
    return NULL;
  }
  json_scanf(json, strlen(json), "{type:%d,label:%Q,icon:%Q}", &type, &label, &icon);
  widget = widget_create(name, x, y, w, h, 0, NULL, NULL);
  if (name) free(name);
  return widget;
}

struct widget_t *widget_create_from_file(const char *fn) {
  char *json;
  struct widget_t *widget=NULL;

  if (!(json = json_fread(fn))) {
    LOG(LL_ERROR, ("%s: Could not json_fread()", fn));
    return NULL;
  }

  widget = widget_create_from_json(json);
  free(json);
  return widget;
}
