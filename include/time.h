#ifndef TERMTRIS_TIME_H_
#define TERMTRIS_TIME_H_

#include <stdbool.h>
#include <sys/time.h>

extern struct timeval time_timeout;

long long time_usec(void);
bool time_update_timeout(void);

#endif
