#include "test.h"
#include "widget.h"

/*
  char *json;
  void *h = NULL;
  struct json_token key, val;
  int idx;

  json = json_fread("data/TestWidget.json");
  if (!json) {
    printf("Could not read json\n");
    return -1;
  }
  
  // Traverse Object
  while ((h = json_next_key(json, strlen(json), h, ".", &key, &val)) != NULL) {
    printf("[%.*s] -> [%.*s]\n", key.len, key.ptr, val.len, val.ptr);
  }

  // Traverse Array
  while ((h = json_next_elem(json, strlen(json), h, ".widgets", &idx, &val)) != NULL) {
    printf("[%d]: [%.*s]\n", idx, val.len, val.ptr);
  }

exit:
  free(json);
  return 0;
*/

static int test_widget_add_from_file(void) {
  struct widget_t *w;
  const char *fn = "data/TestWidget.json";

  w = widget_add_from_file(fn, 0, NULL, NULL);
  if (!w) {
    LOG(LL_ERROR, ("Could not add %s", fn));
    return -1;
  }
  if (16 != w->x || 48 != w->w || 16 != w->y || 48 != w->h) {
    LOG(LL_ERROR, ("widget x, y, w, h incorrect"));
    return -1;
  }

  return 0;
}

int test_widget() {
  test_widget_add_from_file();
  return 0;
}
