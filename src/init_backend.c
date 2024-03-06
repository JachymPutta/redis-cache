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
  dictEntry *de = dbFind(db, key->ptr);
  char req_left_str[12];

  robj *val = NULL;

  if (de) {
    // printf("Rate_limit exists\n");
    val = dictGetVal(de);
    int reqs_left = atoi(val->ptr);
    // printf("Value before decrement: %d\n", reqs_left);
    if (reqs_left > 0) {
      reqs_left--;
      // printf("Value after decrement: %d\n", reqs_left);
      sprintf(req_left_str, "%d", reqs_left);
      robj *new_val = createStringObject(req_left_str, strlen(req_left_str));
      dbReplaceValue(db, key, new_val);
      return 1;
    } 
    else {
      printf("Rate limit exceeded\n");
    }
  } 
  // else {
  //   printf("Rate-limit not found\n");
  //   val = createStringObject(RATE_LIMIT, strlen(RATE_LIMIT));
  //   // robj *dummy_val= createStringObject("10", strlen("10"));
  //   setKey(NULL, db, key, val, SETKEY_DOESNT_EXIST);
  //   setExpire(NULL, db, key, mstime() + RATE_LIMIT_PERIOD_MS);
  // }

  return 0;
}
