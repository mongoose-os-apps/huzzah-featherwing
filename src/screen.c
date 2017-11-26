#include "screen.h"

struct screen_t *screen_create(char *name, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  struct screen_t *screen = NULL;

  screen = (struct screen_t *) calloc(1, sizeof(*screen));
  if (!screen)
    return NULL;
  screen->x=x; 
  screen->y=y; 
  screen->w=w; 
  screen->h=h; 
  if (name)
    screen->name=strdup(name);
  SLIST_INIT(&screen->widget_entries);

  return screen;
}

struct screen_t *screen_create_from_file(char *fn) {
  char *json;
  void *h = NULL;
//  struct json_token key;
  struct json_token val;
  int idx;
  struct screen_t *screen = NULL;
  struct widget_t *widget = NULL;
  int screen_x, screen_y, screen_w, screen_h;
  char *screen_name = NULL;


  json = json_fread(fn);
  if (!json) {
    LOG(LL_ERROR, ("Could not read file %s", fn));
    goto exit;
  }

  if (json_scanf(json, strlen(json), "{x:%d,y:%d,w:%d,h:%d,name:%Q}", &screen_x, &screen_y, &screen_w, &screen_h, &screen_name) != 5) {
    LOG(LL_ERROR, ("%s: Incomplete JSON: require 'x', 'y', 'w', 'h' and 'name' fields", fn));
    screen=NULL; goto exit;
  }
  screen = screen_create(screen_name, screen_x, screen_y, screen_w, screen_h);
  
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
      if (!screen_widget_add(screen, widget)) {
        LOG(LL_ERROR, ("Could not add widget to screen"));
      }
    }
  }

exit:
  if (json) free(json);
  if (screen_name) free(screen_name);
  return screen;
}

void screen_destroy(struct screen_t **s) {
  free(*s);
  if ((*s)->name) free ((*s)->name);
  *s = NULL;
}

bool screen_widget_add(struct screen_t *s, struct widget_t *w) {
  struct widget_list_t *wl;

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
