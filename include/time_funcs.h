#pragma once
#include <time.h>
#include <stdint.h>

/**
 * Get the current system timestamp
 * @param timestamp A timespec pointer
 */
void get_timestamp(struct timespec *timestamp);

/**
 * Get the elapsed time in nanoseconds since the application started
 * @return Elapsed time in nanoseconds
 */
uint64_t get_elapsed_time();

/**
 * Delay for the specified period in milliseconds.
 * @param period The delay period in milliseconds
 * @param timestamp An optional timespec pointer in which the
 * delay will base on
 * @return 0 upon success
 */
int delay_ms(uint32_t period, struct timespec *timestamp);
