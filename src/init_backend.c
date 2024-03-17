 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "init_backend.h"
 #include "server.h"


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

int bwAvailable(redisDb *db) {
  robj *key= createStringObject(server.rate_limit_key, strlen(server.rate_limit_key));
  robj *val = NULL;

  // currently stalls forever on out of bandwidth
  while (1) {
    dictEntry *de = dbFind(db, key->ptr);

    if (de) {
      val = dictGetVal(de);
      int64_t reqs_left = (int64_t) val->ptr;

      if (reqs_left > 0) {
        reqs_left--;
        val->ptr = (void *) (reqs_left);
        return 1;
      } 
    } 

    // printf("looking for key %s\n", server.rate_limit_key);
    usleep(500 * 1000);
  }

  // printf("Rate limit exceeded\n");
  return 0;
}
