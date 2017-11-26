#include "test.h"

int test_widget() {
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
}
