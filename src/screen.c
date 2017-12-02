#include "screen.h"

struct screen_t *screen_create(char *name) {
  struct screen_t *screen = NULL;

  screen = (struct screen_t *) calloc(1, sizeof(*screen));
  if (!screen)
    return NULL;
  if (name)
    screen->name=strdup(name);
  SLIST_INIT(&screen->widget_entries);

  return screen;
}

struct screen_t *screen_create_from_file(char *fn, widget_event_fn handler, void *user_data) {
  char *json;
  struct screen_t *screen = NULL;

  json = json_fread(fn);
  if (!json) {
    LOG(LL_ERROR, ("%s: Could not json_fread()", fn));
    return NULL;
  }

  screen = screen_create_from_json(json, handler, user_data);
  free (json);
  return screen;
}

struct screen_t *screen_create_from_json(char *json, widget_event_fn handler, void *user_data) {
  void *h = NULL;
//  struct json_token key;
  struct json_token val;
  int idx;
  struct screen_t *screen = NULL;
  struct widget_t *widget = NULL;
  char *screen_name = NULL;

  if (json_scanf(json, strlen(json), "{name:%Q}", &screen_name) != 1) {
    LOG(LL_ERROR, ("Incomplete JSON: require 'name' fields"));
    screen=NULL; goto exit;
  }
  screen = screen_create(screen_name);
  if (!screen)
    return NULL;
  screen_widget_set_handler(screen, handler);
  
/*
  // Traverse Object
  while ((h = json_next_key(json, strlen(json), h, ".", &key, &val)) != NULL) {
    printf("[%.*s] -> [%.*s]\n", key.len, key.ptr, val.len, val.ptr);
  }

*/

  // Traverse Array
  while ((h = json_next_elem(json, strlen(json), h, ".widgets", &idx, &val)) != NULL) {
//    printf("[%d]: [%.*s]\n", idx, val.len, val.ptr);
    if (val.len>0 && val.ptr) {
      widget = widget_create_from_json(val.ptr);
      widget_set_handler(widget, screen->default_widget_handler);
      if (!screen_widget_add(screen, widget)) {
        LOG(LL_ERROR, ("Could not add widget to screen"));
      }
    }
  }

exit:
  if (screen_name) free(screen_name);
  return screen;
}

void screen_destroy(struct screen_t **s) {
  if ((*s)->name) free ((*s)->name);
  free(*s);
  *s = NULL;
}

bool screen_widget_add(struct screen_t *s, struct widget_t *w) {
  struct widget_list_t *wl;

  if (!s || !w)
    return false;

  wl = (struct widget_list_t *) calloc(1, sizeof(*wl));
  if (!wl) {
    return false;
  }
  wl->widget = w;
  SLIST_INSERT_HEAD(&s->widget_entries, wl, entries);
  return true;
}

struct widget_t *screen_widget_add_from_file(struct screen_t *s, char *fn) {
  struct widget_t *w;

  w = widget_create_from_file(fn);
  if (!w)
    return NULL;
  widget_set_handler(w, s->default_widget_handler);
  screen_widget_add(s, w);
  return w;
}

bool screen_widget_destroy(struct screen_t *s, struct widget_t **w) {
  struct widget_list_t *wl;

  if (!s)
    return false;
  SLIST_FOREACH(wl, &s->widget_entries, entries) {
    if (wl->widget == *w) {
      SLIST_REMOVE(&s->widget_entries, wl, widget_list_t, entries);
      widget_destroy(w);
    }
  }
  return true;
}

uint16_t screen_get_num_widgets(struct screen_t *s) {
  struct widget_list_t *wl;
  uint16_t num = 0;

  if (!s)
    return 0;
  SLIST_FOREACH(wl, &s->widget_entries, entries)
    num++;
  
  return num;
}

struct widget_t *screen_widget_find_by_xy(struct screen_t *s, uint16_t x, uint16_t y) {
  struct widget_list_t *wl;

  if (!s)
    return NULL;

  SLIST_FOREACH(wl, &s->widget_entries, entries) {
    LOG(LL_DEBUG, ("Inspecing widget '%s' (x=%d,y=%d,w=%d,h=%d)", wl->widget->name, wl->widget->x, wl->widget->y, wl->widget->w, wl->widget->h));
    if (wl->widget->x <= x &&
        x < (wl->widget->x+wl->widget->w) &&
        wl->widget->y <= y &&
        y < (wl->widget->y+wl->widget->h))
      return wl->widget;
  }
  return NULL;
}

void screen_widget_set_handler(struct screen_t *s, widget_event_fn handler) {
  if (!s)
    return;
  s->default_widget_handler=handler;
  return;
}
