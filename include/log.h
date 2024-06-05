#pragma once
#include <err.h>

enum log_level {
    trace,
    debug,
    info,
    warning,
    error
};
// NOLINTBEGIN(readability-function-cognitive-complexity)
#define LOG(level, fmt, ...) if (level == error) warn(fmt, ##__VA_ARGS__); else {printf(fmt, ##__VA_ARGS__); printf("\n");}
// NOLINTEND(readability-function-cognitive-complexity)
