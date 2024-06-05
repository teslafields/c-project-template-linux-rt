#pragma once

/* The task priority: from 0 to 100 */
#define TASK_PRIORITY 90

/* The task period in milliseconds */
#define TASK_PERIOD 1000UL

#define TASK_PERIOD_FROM_MS(ms) (( ms) * 1000 * 1000)

/**
 * Loop function. This function gets called by the default
 * runtime loop, at every TASK_PERIOD period (similar behavior to
 * Arduino's loop function)
 */
void loop();

/**
 * Init the controloop library
 * @return 0 on success
 */
int controloop_init();

/**
 * Block until all spawned tasks complete
 * @return 0 on success
 */
int controloop_join_tasks();

/**
 * Spawn a new task.
 * @param (*task_loop) An optional pointer to a function that is
 * the target task. If this parameter is NULL then the default thread
 * will be created and it will call loop() (see loop())
 * @param data An optional pointer to a data that will be the task
 * argument
 * @return 0 on success
 */
int controloop_spawn_task(void *(*task_loop)(void *data), void *data);
