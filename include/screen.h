#ifndef __SCREEN_H
#define __SCREEN_H

#include "mgos.h"
#include "frozen/frozen.h"
#include "common/queue.h"
#include "widget.h"

struct screen_t {
  char *name;

  // Private
  SLIST_HEAD(widget_entries, widget_list_t) widget_entries;
  widget_event_fn default_widget_handler;
};

struct screen_t *screen_create(char *name);
struct screen_t *screen_create_from_file(char *fn, widget_event_fn handler, void *user_data);
struct screen_t *screen_create_from_json(char *json, widget_event_fn handler, void *user_data);
void screen_destroy(struct screen_t **s);

void screen_widget_set_handler(struct screen_t *s, widget_event_fn handler);

bool screen_widget_add(struct screen_t *s, struct widget_t *w);
struct widget_t *screen_widget_add_from_file(struct screen_t *s, char *fn);

bool screen_widget_destroy(struct screen_t *s, struct widget_t **w);

uint16_t screen_get_num_widgets(struct screen_t *s);

struct widget_t *screen_widget_find_by_xy(struct screen_t *s, uint16_t x, uint16_t y);

void screen_widget_broadcast(struct screen_t *s, int ev, void *ev_data);


#endif //__SCREEN_H
