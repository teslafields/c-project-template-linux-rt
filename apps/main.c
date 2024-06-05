#define _GNU_SOURCE // NOLINT
#include "controloop.h"
#include "log.h"
#include "time_funcs.h"

#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void loop() {
   LOG(debug, "Elapsed: %lu", get_elapsed_time());
}

/* Alternative task to be spawned */
void *main_loop(void *data) {
    (void) data;
    uint64_t now = get_elapsed_time();
    LOG(debug, "I am a new born task!");
    delay_ms(20, NULL);
    LOG(debug, "Took me %lu ns", get_elapsed_time() - now);
    return NULL;
}

int main(int argc, char *argv[]) {
    int ret = 1;

    if (argc > 1) {
        LOG(error, "%s doesn't accept arguments", argv[0]);
        return ret;
    }

    if (controloop_init()) {
        LOG(error, "controloop init failed");
        return 1;
    }

    /* Optionally pass a thread target argument to be called */
    ret = controloop_spawn_task(main_loop, NULL);
    if (ret) {
        LOG(error, "controloop spawn task failed");
        return ret;
    }

    ret = controloop_spawn_task(NULL, NULL);
    if (ret) {
        LOG(error, "controloop spawn task failed");
        return ret;
    }

    /* Block until all threads finish */
    return controloop_join_tasks();
}
