#include <hiredis/hiredis.h>

/* Remote backend stuff */
#define USE_REMOTE_BACKEND 1
#define RATE_LIMIT_KEY "rate_limit"
#define RATE_LIMIT "1"
#define RATE_LIMIT_PERIOD_MS 100000 // 10 seconds

redisContext *connect_to_backend(void);
