#ifndef ZENO_ERROR_REPORTER_H
#define ZENO_ERROR_REPORTER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Logs an error with timestamp, module, and function context.
 *
 * @param module The source file name.
 * @param func The function name where the error occurred.
 * @param message The error message.
 */
void zeno_log_error(const char *module, const char *func, const char *message);

#define ZENO_LOG_ERROR(message) zeno_log_error(__FILE__, __func__, message)

#ifdef __cplusplus
}
#endif

#endif /* ZENO_ERROR_REPORTER_H */
