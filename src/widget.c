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

  if ((*widget)->_timer_id)
    mgos_clear_timer((*widget)->_timer_id);

  if ((*widget)->user_data)
    free((*widget)->user_data);

  if ((*widget)->name)
    free((*widget)->name);

  if ((*widget)->label)
    free((*widget)->label);

  if ((*widget)->img)
    free((*widget)->img);

  free(*widget);
  *widget=NULL;
}

struct widget_t *widget_create(char *name, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  struct widget_t *widget;

  widget = (struct widget_t *) calloc(1, sizeof(*widget));
  if (!widget)
    return NULL;

  widget->name=strdup(name);
  widget->x=x; 
  widget->y=y; 
  widget->w=w; 
  widget->h=h; 

  widget->type=WIDGET_TYPE_NONE; 
  widget->label=NULL;
  widget->img=NULL;
  widget->user_data = NULL;
  widget->handler = NULL;
  widget->timer_msec = 0;
  widget->_timer_id = 0;
  widget->create_called = false;

  return widget;
}

struct widget_t *widget_create_from_json(const char *json) {
  struct widget_t *widget=NULL;
  int x, y, w, h;
  int type = 0;
  char *name = NULL;
  char *label = NULL;
  char *img = NULL;

  if (json_scanf(json, strlen(json), "{name:%Q,x:%d,y:%d,w:%d,h:%d}", &name, &x, &y, &w, &h) != 5) {
    LOG(LL_ERROR, ("Incomplete JSON: require 'x', 'y', 'w', 'h' and 'name' fields"));
    return NULL;
  }
  widget = widget_create(name, x, y, w, h);
  free(name);

  json_scanf(json, strlen(json), "{type:%d,label:%Q,img:%Q}", &type, &label, &img);
  widget->type=type;
  widget->label=label;
  widget->img=img;

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

void widget_set_handler(struct widget_t *w, widget_event_fn handler, void *user_data) {
  if (!w)
    return;
  w->handler = handler;
  w->user_data = user_data;
  if (!w->create_called && w->handler)
    handler(EV_WIDGET_CREATE, w, NULL);
    
  return;
}

void widget_delete_handler(struct widget_t *w) {
  if (!w)
    return;
  w->handler = NULL;
  return;
}

void widget_set_timer(struct widget_t *w, uint32_t timer_msec) {
  if (!w)
    return;
  if (w->_timer_id)
    mgos_clear_timer(w->_timer_id);

  w->timer_msec=timer_msec; 
  if (timer_msec == 0)
    w->_timer_id = 0;
  else 
    w->_timer_id = mgos_set_timer(timer_msec, MGOS_TIMER_REPEAT, widget_event_timer, w);

  return;
}

void widget_delete_timer(struct widget_t *w) {
  if (!w)
    return;

  widget_set_timer(w, 0);
  return;
}

void widget_ev_to_str(int ev, char *s, int slen) {
  switch(ev) {
    case EV_WIDGET_CREATE:
      strncpy(s, "CREATE", slen);
      break;
    case EV_WIDGET_DRAW:
      strncpy(s, "DRAW", slen);
      break;
    case EV_WIDGET_REDRAW:
      strncpy(s, "REDRAW", slen);
      break;
    case EV_WIDGET_TIMER:
      strncpy(s, "TIMER", slen);
      break;
    case EV_WIDGET_TOUCH_UP:
      strncpy(s, "TOUCH_UP", slen);
      break;
    case EV_WIDGET_TOUCH_DOWN:
      strncpy(s, "TOUCH_DOWN", slen);
      break;
    case EV_WIDGET_DESTROY:
      strncpy(s, "DESTROY", slen);
      break;
    default: // EV_WIDGET_NONE
      snprintf(s, slen, "EV%d", ev);
      break;
  }
}
