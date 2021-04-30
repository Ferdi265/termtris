#include <stddef.h>
#include "time.h"

struct timeval time_timeout;

long long time_usec(void) {
    struct timeval time;
    gettimeofday(&time, NULL);

    return time.tv_sec * 1000000LL + time.tv_usec;
}

long long time_rate = 16666;
long long time_now;
struct timeval time_timeout;
bool time_update_timeout(void) {
    long long now = time_usec();
    long long diff = now - time_now;
    time_now = now;

    if (diff > time_timeout.tv_usec) {
        time_timeout.tv_usec = time_rate;
        return true;
    } else {
        time_timeout.tv_usec -= diff;
        return false;
    }
}
