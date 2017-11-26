#ifndef __WIDGET_H
#define __WIDGET_H

#include "common/queue.h"

struct widget_t;

#define EV_WIDGET_NONE       0
#define EV_WIDGET_CREATE     1
#define EV_WIDGET_DESTROY    2
#define EV_WIDGET_DRAW       3
#define EV_WIDGET_REDRAW     4
#define EV_WIDGET_TIMER      5
#define EV_WIDGET_TOUCH_UP   6 // struct mgos_stmpe610_event_data *
#define EV_WIDGET_TOUCH_DOWN 7 // struct mgos_stmpe610_event_data *

enum widget_type_t {
  WIDGET_TYPE_NONE           =0,
  WIDGET_TYPE_IMAGE          =1,
  WIDGET_TYPE_LOADSCREEN     =2,
};

typedef void (*widget_event_fn)(int ev, struct widget_t *w, void *ev_data);

struct widget_t {
  char *name;
  uint16_t x, y, w, h;

  enum widget_type_t type;
  char *label;
  char *img;

  uint32_t timer_msec;      // 0 to disable
  widget_event_fn handler;  // Event callback for this widget
  void *user_data;          // User supplied data

  // Private
  mgos_timer_id _timer_id;
  uint8_t create_called;
};

struct widget_list_t {
  struct widget_t *widget;
  SLIST_ENTRY(widget_list_t) entries;
};

struct widget_t *widget_create(char *name, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void widget_set_handler(struct widget_t *w, widget_event_fn handler, void *user_data);
void widget_delete_handler(struct widget_t *w);
void widget_set_timer(struct widget_t *w, uint32_t timer_msec);
void widget_delete_timer(struct widget_t *w);
struct widget_t *widget_create_from_json(const char *json);
struct widget_t *widget_create_from_file(const char *fn);
void widget_destroy(struct widget_t **widget);

// Convert ev in EV_WIDGET_* to string
void widget_ev_to_str(int ev, char *s, int slen);


#endif // __WIDGET_H
