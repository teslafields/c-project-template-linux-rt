/*
 * Example available on: https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
 *
 * POSIX Real Time Example
 * using a single pthread as RT thread
 */

#include "controloop.h"
#include "log.h"
#include "time_funcs.h"

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdbool.h>

#ifdef CAP_SUPPORT
    #ifdef defined(__has_include)
        #if __has_include(<linux/capability.h>)
            #include <linux/capability.h>
        #else
            #include <sys/capability.h>
        #endif
    #else
        #include <sys/capability.h>
    #endif
#endif

#define NR_OF_TASKS (2)

static pthread_t task_arr[NR_OF_TASKS] = {0};

static bool RUN_LOOP = true;

void handle_sigint(int signum) {
    (void) signum;
    RUN_LOOP = false;
}

#ifdef CAP_SUPPORT
static int capabilities_setup() {
    int ret = 0;
    const cap_value_t cap_list[1] = { CAP_SYS_NICE };
    cap_t caps;

    caps = cap_get_proc();
    if (caps == NULL) {
        LOG(error, "cap get failed");
        return -1;
    }

    ret = cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, CAP_SET);
    if (ret) {
        LOG(error, "cap set flag CAP_SYS_NICE failed");
        return ret;
    }

    ret = cap_set_proc(caps);
    if (ret) {
        LOG(error, "cap set thread capabilities failed");
        return ret;
    }

    ret = cap_free(caps);
    if (ret) {
        LOG(error, "cap free failed");
    }
    return ret;
}
#endif

__attribute__ ((weak)) void loop() {
    /* Weak reference of loop() here */
}

static void *runtime_loop(void *data) {
    (void) data;
    struct timespec timestamp;

    while (RUN_LOOP) {
        get_timestamp(&timestamp);

        /* Do RT specific stuff here */
        loop();

        delay_ms(TASK_PERIOD, &timestamp);
    }

    LOG(warning, "Received interrupt signal");
    return NULL;
}

int controloop_spawn_task(void *(*task_loop)(void *data), void *data) { // NOLINT
    static int task_idx = 0;

    if (task_idx >= NR_OF_TASKS) {
        LOG(error, "Max number of tasks reached");
        return -1;
    }

    int ret;
    struct sched_param param;
    pthread_attr_t attr;

#ifdef CAP_SUPPORT
    if (capabilities_setup()) {
        exit(-2);
    }
#endif

    /* Lock memory */
    if (mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        LOG(error, "mlockall failed");
        exit(-2);
    }

    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
        LOG(error, "init pthread attributes failed");
        return ret;
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
        LOG(error, "pthread setstacksize failed");
        return ret;
    }

    /* Set scheduler policy of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
        LOG(error, "pthread setschedpolicy failed");
        return ret;
    }

    /* Set scheduler priority of pthread */
    param.sched_priority = TASK_PRIORITY;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
        LOG(error, "pthread setschedparam failed");
        return ret;
    }

    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        LOG(error, "pthread setinheritsched failed");
    }

    /* Create a pthread with specified attributes */
    if (task_loop) {
        ret = pthread_create(&task_arr[task_idx++], &attr, task_loop, data);
    } else {
        ret = pthread_create(&task_arr[task_idx++], &attr, runtime_loop, data);
    }
    if (ret) {
        LOG(error, "create pthread failed");
        return ret;
    }

    return ret;
}

int controloop_join_tasks() {
    int ret = 0;

    for (int i = 0; i < NR_OF_TASKS; i++) {
        if (!task_arr[i]) {
            continue;
        }
        /* Join the thread and wait until it is done */
        ret = pthread_join(task_arr[i], NULL);
        if (ret) {
            LOG(error, "join pthread failed: %m");
        }
        task_arr[i] = 0;
    }

    return ret;
}

int controloop_init() {

    /* Do global configurations here */
   signal(SIGINT, handle_sigint);

   return 0;
}

void print_tasks() {
    for (int i = 0; i < NR_OF_TASKS; i++) {
        if (task_arr[i]) {
            LOG(debug, "Task %lu is active", task_arr[i]);
        } else {
            LOG(debug, "Task %lu is inactive", task_arr[i]);
        }
    }
}
