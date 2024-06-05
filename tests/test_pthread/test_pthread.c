#define _GNU_SOURCE // NOLINT
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <cmocka.h>

/* include here your files that contain test functions */
#include "controloop.h"

static struct sched_param thread_sp;
static int sched_policy, sched_inherit;
static size_t stack_size;

// NOLINTBEGIN(*)
static void display_pthread_attr(pthread_attr_t *attr, char *prefix)
{
    int ret, i;
    size_t v;
    void *stkaddr;

    ret = pthread_attr_getdetachstate(attr, &i);
    if (ret == 0) {
        printf("%sDetach state        = %s\n", prefix,
                (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :
                (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :
                "???");
    }

    ret = pthread_attr_getscope(attr, &i);
    if (ret == 0) {
        printf("%sScope               = %s\n", prefix,
                (i == PTHREAD_SCOPE_SYSTEM)  ? "PTHREAD_SCOPE_SYSTEM" :
                (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :
                "???");
    }

    ret = pthread_attr_getinheritsched(attr, &i);
    if (ret == 0) {
        printf("%sInherit scheduler   = %s\n", prefix,
                (i == PTHREAD_INHERIT_SCHED)  ? "PTHREAD_INHERIT_SCHED" :
                (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED" :
                "???");
        sched_inherit = i;
    }

    ret = pthread_attr_getschedpolicy(attr, &i);
    if (ret == 0) {
        printf("%sScheduling policy   = %s\n", prefix,
                (i == SCHED_OTHER) ? "SCHED_OTHER" :
                (i == SCHED_FIFO)  ? "SCHED_FIFO" :
                (i == SCHED_RR)    ? "SCHED_RR" :
                "???");
        sched_policy = i;
    }

    ret = pthread_attr_getschedparam(attr, &thread_sp);
    if (ret == 0) {
        printf("%sScheduling priority = %d\n", prefix, thread_sp.sched_priority);
    }

    ret = pthread_attr_getguardsize(attr, &v);
    if (ret == 0) {
        printf("%sGuard size          = %zu bytes\n", prefix, v);
    }

    ret = pthread_attr_getstack(attr, &stkaddr, &v);
    if (ret == 0) {
        printf("%sStack address       = %p\n", prefix, stkaddr);
        printf("%sStack size          = %#zx bytes\n", prefix, v);
        stack_size = v;
    }
}
// NOLINTEND(*)

static void *thread_start(void *arg) {
    (void) arg;

    int ret;
    pthread_attr_t gattr;

    /* pthread_getattr_np() is a non-standard GNU extension that
       retrieves the attributes of the thread specified in its
       first argument. */

    ret = pthread_getattr_np(pthread_self(), &gattr);
    if (ret == 0) {
        display_pthread_attr(&gattr, "\t");
    }

    return NULL;
}

static void test_pthread_creation(void **state) {
    (void) state;
    int ret = controloop_spawn_task(thread_start, NULL);
    assert_int_equal(ret, 0);
}

static void test_pthread_parameters(void **state) {
    (void) state;

    int ret = controloop_join_tasks();
    assert_int_equal(ret, 0);

    assert_int_equal(thread_sp.sched_priority, TASK_PRIORITY);
    assert_int_equal(sched_policy, SCHED_FIFO);
    assert_int_equal(sched_inherit, PTHREAD_EXPLICIT_SCHED);
    assert_int_equal(stack_size, PTHREAD_STACK_MIN);
}

/**
 * Test runner function
 */
int main(void) {

    /**
     * Insert here your test functions
     */
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_pthread_creation),
        cmocka_unit_test(test_pthread_parameters),
    };


    /* Run the tests */
    return cmocka_run_group_tests(tests, NULL, NULL);
}
