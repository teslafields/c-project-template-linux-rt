#include "controloop.h"
#include "time_funcs.h"

#define TIMESPEC_TO_NS(ts) ((ts).tv_sec * 1000000000 + (ts).tv_nsec)

void get_timestamp(struct timespec *timestamp) {
    /* get timestamp before */
    clock_gettime(CLOCK_MONOTONIC, timestamp);
}

uint64_t get_elapsed_time() {
    static uint64_t elapsed = 0;
    struct timespec timestamp;
    get_timestamp(&timestamp);
    if (elapsed == 0) {
        elapsed = TIMESPEC_TO_NS(timestamp);
    }
    return TIMESPEC_TO_NS(timestamp) - elapsed;
}

int delay_ms(uint32_t period, struct timespec *timestamp) {
    struct timespec local_ts;

    if (!timestamp) {
        clock_gettime(CLOCK_MONOTONIC, &local_ts);
        timestamp = &local_ts;
    }

    /* Add the time you want to sleep */
    timestamp->tv_nsec += (long) TASK_PERIOD_FROM_MS(period);

    /* Normalize the time to account for the second boundary */
    if(timestamp->tv_nsec >= 1000000000) {
        timestamp->tv_nsec -= 1000000000;
        timestamp->tv_sec++;
    }

    return clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, timestamp, NULL);
}
