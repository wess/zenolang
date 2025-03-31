#include "error_reporter.h"
#include <time.h>
#include <stdlib.h>

/**
 * Logs an error with timestamp, module, and function context.
 *
 * Uses the current local time to generate a timestamp and prints
 * a formatted error message to stderr.
 */
void zeno_log_error(const char *module, const char *func, const char *message) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    if (!local) {
        fprintf(stderr, "Failed to obtain local time\n");
        return;
    }
    
    char time_str[20]; // Format: YYYY-MM-DD HH:MM:SS
    if (strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local) == 0) {
        fprintf(stderr, "Failed to format time string\n");
        return;
    }
    
    fprintf(stderr, "[%s] Error in %s:%s -> %s\n", time_str, module, func, message);
}
