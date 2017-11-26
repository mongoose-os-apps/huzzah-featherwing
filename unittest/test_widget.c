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

static int test_widget_create_from_file(char *fn) {
  struct widget_t *w;

  LOG(LL_INFO, ("Adding widget from %s", fn));
  w = widget_create_from_file(fn);
  ASSERT(w, "widget_create_from_file()");
  ASSERT(w->x == 16, "'x' field is invalid");
  ASSERT(w->y == 16, "'x' field is invalid");
  ASSERT(w->w == 48, "'x' field is invalid");
  ASSERT(w->h == 48, "'x' field is invalid");

  return 0;
}

int test_widget() {
  test_widget_create_from_file("data/TestWidget.json");
  return 0;
}
