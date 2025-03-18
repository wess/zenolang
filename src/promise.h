/**
 * @file promise.h
 * @brief Promise implementation similar to JavaScript Promises
 */

#ifndef ZENO_PROMISE_H
#define ZENO_PROMISE_H

#include "threads.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Promise states
 */
typedef enum {
    ZN_PROMISE_PENDING,
    ZN_PROMISE_FULFILLED,
    ZN_PROMISE_REJECTED
} zn_promise_state_t;

/**
 * @brief Promise structure (opaque type)
 */
typedef struct zn_promise zn_promise_t;

/**
 * @brief Promise resolver function type
 */
typedef void (*zn_promise_resolver_t)(void (*resolve)(void *value), 
                                     void (*reject)(void *error), 
                                     void *context);

/**
 * @brief Fulfill handler function type
 */
typedef void *(*zn_then_handler_t)(void *value);

/**
 * @brief Reject handler function type
 */
typedef void *(*zn_catch_handler_t)(void *error);

/**
 * @brief Finally handler function type
 */
typedef void (*zn_finally_handler_t)(void);

/**
 * @brief Create a new promise
 * @param resolver Function that will try to resolve the promise
 * @param context User context for the resolver
 * @return New promise
 */
zn_promise_t *zn_promise_new(zn_promise_resolver_t resolver, void *context);

/**
 * @brief Create a resolved promise
 * @param value Value to resolve with
 * @return New resolved promise
 */
zn_promise_t *zn_promise_resolve(void *value);

/**
 * @brief Create a rejected promise
 * @param error Error to reject with
 * @return New rejected promise
 */
zn_promise_t *zn_promise_reject(void *error);

/**
 * @brief Chain a fulfillment handler to a promise
 * @param promise The promise
 * @param on_fulfilled Handler for fulfillment
 * @return A new promise
 */
zn_promise_t *zn_promise_then(zn_promise_t *promise, zn_then_handler_t on_fulfilled);

/**
 * @brief Chain a rejection handler to a promise
 * @param promise The promise
 * @param on_rejected Handler for rejection
 * @return A new promise
 */
zn_promise_t *zn_promise_catch(zn_promise_t *promise, zn_catch_handler_t on_rejected);

/**
 * @brief Add a finally handler to a promise
 * @param promise The promise
 * @param on_finally Handler to run when promise settles
 * @return A new promise
 */
zn_promise_t *zn_promise_finally(zn_promise_t *promise, zn_finally_handler_t on_finally);

/**
 * @brief Wait for a promise to settle
 * @param promise The promise
 * @return Value if fulfilled, NULL if rejected
 */
void *zn_promise_await(zn_promise_t *promise);

/**
 * @brief Returns the current state of the promise
 * @param promise The promise
 * @return Current state
 */
zn_promise_state_t zn_promise_state(zn_promise_t *promise);

/**
 * @brief Free resources used by a promise
 * @param promise The promise
 */
void zn_promise_free(zn_promise_t *promise);

/**
 * @brief Macro to declare an async function
 * @param return_type Return type of the function
 * @param name Function name
 * @param ... Function parameters
 */
#define ZN_ASYNC(return_type, name, ...) \
    zn_promise_t *name(__VA_ARGS__)

/**
 * @brief Macro to create a new async task from a function
 * @param func Function to execute asynchronously
 * @param ... Function arguments
 * @return Promise that will be fulfilled with the function result
 */
#define zn_async(func, ...) \
    _zn_async_call((void *(*)(void *))func, _zn_async_pack(__VA_ARGS__))

/* Helper function for zn_async macro */
zn_promise_t *_zn_async_call(void *(*func)(void *), void *args);

/* Helper function for packing arguments */
void *_zn_async_pack(void *arg, ...);

/**
 * @brief Executes a promise and waits for its result
 * @param promise The promise to await
 * @return Result of the promise
 */
#define zn_await(promise) \
    zn_promise_await(promise)

#endif /* ZENO_PROMISE_H */
