 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "init_backend.h"
 #include "server.h"


redisContext *connect_to_backend(void) {
  char REMOTE_BACKEND_IP[] = "127.0.0.1";
  int REMOTE_BACKEND_PORT = 6379;
  redisContext *c;

  c = redisConnect(REMOTE_BACKEND_IP, REMOTE_BACKEND_PORT);
  if (c->err) {
      // printf("error: %s\n", c->errstr);
      return NULL;
  }

  return c;
}

int bwAvailable(redisDb *db) {
  robj *key= createStringObject(RATE_LIMIT_KEY, strlen(RATE_LIMIT_KEY));
  dictEntry *de = dbFind(db, key->ptr);
  char req_left_str[12];

  robj *val = NULL;

  if (de) {
    printf("Rate_limit exists\n");
    val = dictGetVal(de);
    int reqs_left = atoi(val->ptr);
    printf("Value before decrement: %d\n", reqs_left);
    if (reqs_left > 0) {
      reqs_left--;
      printf("Value after decrement: %d\n", reqs_left);
      sprintf(req_left_str, "%d", reqs_left);
      robj *new_val = createStringObject(req_left_str, strlen(req_left_str));
      dbReplaceValue(db, key, new_val);
    } else {
      printf("Rate limit exceeded\n");
      return 0;
    }
  } else {
    printf("Rate-limit not found\n");
    val = createStringObject(RATE_LIMIT, strlen(RATE_LIMIT));
    // robj *dummy_val= createStringObject("10", strlen("10"));
    setKey(NULL, db, key, val, SETKEY_DOESNT_EXIST);
    setExpire(NULL, db, key, mstime() + RATE_LIMIT_PERIOD_MS);
  }

  return 1;
}
