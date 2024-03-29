#include <hiredis/hiredis.h>

/* Remote backend stuff */
#define USE_REMOTE_BACKEND 1

redisContext *connect_to_backend(void);
