#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init_backend.h"
#include "server.h"
#include "sds.h"


redisContext *connect_to_backend(void) {
  int REMOTE_BACKEND_PORT = 6379;
  redisContext *c;

  c = redisConnect(server.remote_backend_ip, REMOTE_BACKEND_PORT);
  if (c->err) {
      // printf("error: %s\n", c->errstr);
      return NULL;
  }

  return c;
}

int bwAvailable(redisDb *db, int isEviction) {
  robj *key= createStringObject(server.rate_limit_key, strlen(server.rate_limit_key));
  robj *val = NULL;

  // currently stalls forever on out of bandwidth
  while (1) {
    val = lookupKeyWrite(db, key);

    if (val) {
      int64_t reqs_left = (int64_t) val->ptr;

      if (reqs_left > 0) {
        reqs_left--;
        val->ptr = (void *) (reqs_left);
        return 1;
      } 
    } 

    if (isEviction) {
      return 1;
    }
    // printf("Stuck here");
    // usleep(500 * 1000);
  }

  // printf("Rate limit exceeded\n");
  return 0;
}

int isRateLimKey(void *key_ptr) {
  sds key_str = sdsnew(key_ptr);
  sds rate_lim_str = sdsnew(server.rate_limit_key);

  // printf("key_str: %s rate_lim_str: %s\n", key_str, rate_lim_str);
  // printf("strcmp: %d\n", strcmp(key_str, rate_lim_str));

  return strcmp(key_str, rate_lim_str) == 0;
}
