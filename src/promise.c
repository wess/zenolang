/**
 * @file promise.c
 * @brief Implementation of the Promise API
 */

#include "promise.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Maximum number of handlers in a chain */
#define MAX_HANDLERS 32

/* Promise handler types */
typedef enum {
    HANDLER_THEN,
    HANDLER_CATCH,
    HANDLER_FINALLY
} handler_type_t;

/* Promise handler structure */
typedef struct {
    handler_type_t type;
    union {
        zn_then_handler_t then_fn;
        zn_catch_handler_t catch_fn;
        zn_finally_handler_t finally_fn;
    } handler;
    zn_promise_t *next_promise;
} promise_handler_t;

/* Promise structure */
struct zn_promise {
    zn_promise_state_t state;
    void *value;
    void *error;
    
    /* Synchronization */
    zn_mutex_t mutex;
    zn_cond_t cond;
    
    /* Handlers */
    promise_handler_t handlers[MAX_HANDLERS];
    int handler_count;
    
    /* Async task */
    zn_thread_t thread;
    bool has_thread;
};

/* Argument pack structure for async functions */
typedef struct {
    int num_args;
    void *args[10]; /* Support up to 10 arguments */
} arg_pack_t;

/* Resolver context structure */
typedef struct {
    zn_promise_resolver_t resolver;
    void *context;
    zn_promise_t *promise;
} resolver_context_t;

/* Forward declarations for internal functions */
static void promise_resolve_internal(zn_promise_t *promise, void *value);
static void promise_reject_internal(zn_promise_t *promise, void *error);
static void promise_execute_handlers(zn_promise_t *promise);
static void *promise_thread_func(void *arg);
static void *resolver_thread(void *arg);

/* Helper function to get thread-specific data */
void *zn_thread_self_data(void) {
    /* This would require thread-specific storage implementation in threads.h/c */
    /* For now just return NULL as a placeholder */
    return NULL;
}

/* Helper function to set thread-specific data */
void zn_thread_set_data(zn_thread_t *thread, void *data) {
    /* This would require thread-specific storage implementation in threads.h/c */
    /* For now just a placeholder */
    (void)thread;
    (void)data;
}

/* Resolver callback wrappers */
static void resolve_callback(void *value) {
    zn_promise_t *promise = zn_thread_self_data();
    promise_resolve_internal(promise, value);
}

static void reject_callback(void *error) {
    zn_promise_t *promise = zn_thread_self_data();
    promise_reject_internal(promise, error);
}

/* Thread function for resolver */
static void *resolver_thread(void *arg) {
    resolver_context_t *ctx = (resolver_context_t *)arg;
    ctx->resolver(resolve_callback, reject_callback, ctx->context);
    free(ctx);
    return NULL;
}

zn_promise_t *zn_promise_new(zn_promise_resolver_t resolver, void *context) {
    if (!resolver) {
        return NULL;
    }
    
    zn_promise_t *promise = (zn_promise_t *)malloc(sizeof(zn_promise_t));
    if (!promise) {
        return NULL;
    }
    
    memset(promise, 0, sizeof(zn_promise_t));
    promise->state = ZN_PROMISE_PENDING;
    
    if (zn_mutex_init(&promise->mutex) != 0 || 
        zn_cond_init(&promise->cond) != 0) {
        free(promise);
        return NULL;
    }
    
    /* Prepare and start the resolver thread */
    if (zn_thread_init(&promise->thread) != 0) {
        zn_mutex_destroy(&promise->mutex);
        zn_cond_destroy(&promise->cond);
        free(promise);
        return NULL;
    }
    
    /* Store promise in thread for resolver callbacks */
    zn_thread_set_data(&promise->thread, promise);
    
    resolver_context_t *ctx = (resolver_context_t *)malloc(sizeof(resolver_context_t));
    if (!ctx) {
        zn_mutex_destroy(&promise->mutex);
        zn_cond_destroy(&promise->cond);
        free(promise);
        return NULL;
    }
    
    ctx->resolver = resolver;
    ctx->context = context;
    ctx->promise = promise;
    
    if (zn_thread_create(&promise->thread, resolver_thread, ctx) != 0) {
        zn_mutex_destroy(&promise->mutex);
        zn_cond_destroy(&promise->cond);
        free(ctx);
        free(promise);
        return NULL;
    }
    
    promise->has_thread = true;
    return promise;
}

zn_promise_t *zn_promise_resolve(void *value) {
    zn_promise_t *promise = (zn_promise_t *)malloc(sizeof(zn_promise_t));
    if (!promise) {
        return NULL;
    }
    
    memset(promise, 0, sizeof(zn_promise_t));
    promise->state = ZN_PROMISE_FULFILLED;
    promise->value = value;
    
    if (zn_mutex_init(&promise->mutex) != 0 || 
        zn_cond_init(&promise->cond) != 0) {
        free(promise);
        return NULL;
    }
    
    /* Signal that promise is already fulfilled */
    zn_cond_broadcast(&promise->cond);
    
    return promise;
}

zn_promise_t *zn_promise_reject(void *error) {
    zn_promise_t *promise = (zn_promise_t *)malloc(sizeof(zn_promise_t));
    if (!promise) {
        return NULL;
    }
    
    memset(promise, 0, sizeof(zn_promise_t));
    promise->state = ZN_PROMISE_REJECTED;
    promise->error = error;
    
    if (zn_mutex_init(&promise->mutex) != 0 || 
        zn_cond_init(&promise->cond) != 0) {
        free(promise);
        return NULL;
    }
    
    /* Signal that promise is already rejected */
    zn_cond_broadcast(&promise->cond);
    
    return promise;
}

static void promise_resolve_internal(zn_promise_t *promise, void *value) {
    zn_mutex_lock(&promise->mutex);
    
    /* Only pending promises can be resolved */
    if (promise->state == ZN_PROMISE_PENDING) {
        promise->state = ZN_PROMISE_FULFILLED;
        promise->value = value;
        
        /* Signal to any awaiting threads */
        zn_cond_broadcast(&promise->cond);
        
        /* Execute handlers */
        promise_execute_handlers(promise);
    }
    
    zn_mutex_unlock(&promise->mutex);
}

static void promise_reject_internal(zn_promise_t *promise, void *error) {
    zn_mutex_lock(&promise->mutex);
    
    /* Only pending promises can be rejected */
    if (promise->state == ZN_PROMISE_PENDING) {
        promise->state = ZN_PROMISE_REJECTED;
        promise->error = error;
        
        /* Signal to any awaiting threads */
        zn_cond_broadcast(&promise->cond);
        
        /* Execute handlers */
        promise_execute_handlers(promise);
    }
    
    zn_mutex_unlock(&promise->mutex);
}

static void promise_execute_handlers(zn_promise_t *promise) {
    for (int i = 0; i < promise->handler_count; i++) {
        promise_handler_t *handler = &promise->handlers[i];
        zn_promise_t *next_promise = handler->next_promise;
        
        if (!next_promise) {
            continue;
        }
        
        /* Handle based on promise state and handler type */
        if (promise->state == ZN_PROMISE_FULFILLED) {
            if (handler->type == HANDLER_THEN) {
                /* Execute then handler */
                void *result = NULL;
                if (handler->handler.then_fn) {
                    result = handler->handler.then_fn(promise->value);
                } else {
                    /* If no handler, pass through the value */
                    result = promise->value;
                }
                promise_resolve_internal(next_promise, result);
            } else if (handler->type == HANDLER_FINALLY) {
                /* Execute finally handler */
                if (handler->handler.finally_fn) {
                    handler->handler.finally_fn();
                }
                /* Pass through value */
                promise_resolve_internal(next_promise, promise->value);
            } else {
                /* Skip catch handler, pass through value */
                promise_resolve_internal(next_promise, promise->value);
            }
        } else if (promise->state == ZN_PROMISE_REJECTED) {
            if (handler->type == HANDLER_CATCH) {
                /* Execute catch handler */
                void *result = NULL;
                if (handler->handler.catch_fn) {
                    result = handler->handler.catch_fn(promise->error);
                    /* Recovered from error */
                    promise_resolve_internal(next_promise, result);
                } else {
                    /* No handler, propagate the error */
                    promise_reject_internal(next_promise, promise->error);
                }
            } else if (handler->type == HANDLER_FINALLY) {
                /* Execute finally handler */
                if (handler->handler.finally_fn) {
                    handler->handler.finally_fn();
                }
                /* Pass through error */
                promise_reject_internal(next_promise, promise->error);
            } else {
                /* Skip then handler, propagate error */
                promise_reject_internal(next_promise, promise->error);
            }
        }
    }
}

zn_promise_t *zn_promise_then(zn_promise_t *promise, zn_then_handler_t on_fulfilled) {
    if (!promise) {
        return NULL;
    }
    
    zn_promise_t *next_promise = (zn_promise_t *)malloc(sizeof(zn_promise_t));
    if (!next_promise) {
        return NULL;
    }
    
    memset(next_promise, 0, sizeof(zn_promise_t));
    next_promise->state = ZN_PROMISE_PENDING;
    
    if (zn_mutex_init(&next_promise->mutex) != 0 || 
        zn_cond_init(&next_promise->cond) != 0) {
        free(next_promise);
        return NULL;
    }
    
    zn_mutex_lock(&promise->mutex);
    
    if (promise->handler_count < MAX_HANDLERS) {
        /* Add then handler */
        promise_handler_t *handler = &promise->handlers[promise->handler_count++];
        handler->type = HANDLER_THEN;
        handler->handler.then_fn = on_fulfilled;
        handler->next_promise = next_promise;
        
        /* If promise already fulfilled, execute handler immediately */
        if (promise->state != ZN_PROMISE_PENDING) {
            promise_execute_handlers(promise);
        }
    }
    
    zn_mutex_unlock(&promise->mutex);
    
    return next_promise;
}

zn_promise_t *zn_promise_catch(zn_promise_t *promise, zn_catch_handler_t on_rejected) {
    if (!promise) {
        return NULL;
    }
    
    zn_promise_t *next_promise = (zn_promise_t *)malloc(sizeof(zn_promise_t));
    if (!next_promise) {
        return NULL;
    }
    
    memset(next_promise, 0, sizeof(zn_promise_t));
    next_promise->state = ZN_PROMISE_PENDING;
    
    if (zn_mutex_init(&next_promise->mutex) != 0 || 
        zn_cond_init(&next_promise->cond) != 0) {
        free(next_promise);
        return NULL;
    }
    
    zn_mutex_lock(&promise->mutex);
    
    if (promise->handler_count < MAX_HANDLERS) {
        /* Add catch handler */
        promise_handler_t *handler = &promise->handlers[promise->handler_count++];
        handler->type = HANDLER_CATCH;
        handler->handler.catch_fn = on_rejected;
        handler->next_promise = next_promise;
        
        /* If promise already rejected, execute handler immediately */
        if (promise->state != ZN_PROMISE_PENDING) {
            promise_execute_handlers(promise);
        }
    }
    
    zn_mutex_unlock(&promise->mutex);
    
    return next_promise;
}

zn_promise_t *zn_promise_finally(zn_promise_t *promise, zn_finally_handler_t on_finally) {
    if (!promise) {
        return NULL;
    }
    
    zn_promise_t *next_promise = (zn_promise_t *)malloc(sizeof(zn_promise_t));
    if (!next_promise) {
        return NULL;
    }
    
    memset(next_promise, 0, sizeof(zn_promise_t));
    next_promise->state = ZN_PROMISE_PENDING;
    
    if (zn_mutex_init(&next_promise->mutex) != 0 || 
        zn_cond_init(&next_promise->cond) != 0) {
        free(next_promise);
        return NULL;
    }
    
    zn_mutex_lock(&promise->mutex);
    
    if (promise->handler_count < MAX_HANDLERS) {
        /* Add finally handler */
        promise_handler_t *handler = &promise->handlers[promise->handler_count++];
        handler->type = HANDLER_FINALLY;
        handler->handler.finally_fn = on_finally;
        handler->next_promise = next_promise;
        
        /* If promise already settled, execute handler immediately */
        if (promise->state != ZN_PROMISE_PENDING) {
            promise_execute_handlers(promise);
        }
    }
    
    zn_mutex_unlock(&promise->mutex);
    
    return next_promise;
}

void *zn_promise_await(zn_promise_t *promise) {
    if (!promise) {
        return NULL;
    }
    
    zn_mutex_lock(&promise->mutex);
    
    /* Wait while promise is pending */
    while (promise->state == ZN_PROMISE_PENDING) {
        zn_cond_wait(&promise->cond, &promise->mutex);
    }
    
    void *result = (promise->state == ZN_PROMISE_FULFILLED) ? 
                     promise->value : NULL;
    
    zn_mutex_unlock(&promise->mutex);
    
    return result;
}

zn_promise_state_t zn_promise_state(zn_promise_t *promise) {
    if (!promise) {
        return ZN_PROMISE_REJECTED;
    }
    
    zn_mutex_lock(&promise->mutex);
    zn_promise_state_t state = promise->state;
    zn_mutex_unlock(&promise->mutex);
    
    return state;
}

void zn_promise_free(zn_promise_t *promise) {
    if (!promise) {
        return;
    }
    
    /* Wait for promise thread to complete */
    if (promise->has_thread) {
        zn_thread_join(&promise->thread, NULL);
    }
    
    zn_mutex_destroy(&promise->mutex);
    zn_cond_destroy(&promise->cond);
    
    free(promise);
}

/* Helper function to pack arguments */
void *_zn_async_pack(void *arg, ...) {
    arg_pack_t *pack = (arg_pack_t *)malloc(sizeof(arg_pack_t));
    if (!pack) {
        return NULL;
    }
    
    memset(pack, 0, sizeof(arg_pack_t));
    
    /* First argument */
    if (arg) {
        pack->args[0] = arg;
        pack->num_args = 1;
    }
    
    /* Parse variable arguments */
    va_list args;
    va_start(args, arg);
    
    for (int i = 1; i < 10; i++) {
        void *next_arg = va_arg(args, void *);
        if (!next_arg) {
            break;
        }
        
        pack->args[i] = next_arg;
        pack->num_args++;
    }
    
    va_end(args);
    
    return pack;
}

/* Helper structure for async call */
typedef struct {
    void *(*func)(void *);
    void *args;
    zn_promise_t *promise;
} async_call_ctx_t;

/* Thread function for async call */
static void *async_thread_func(void *arg) {
    async_call_ctx_t *ctx = (async_call_ctx_t *)arg;
    void *result = ctx->func(ctx->args);
    
    /* Resolve the promise with the result */
    promise_resolve_internal(ctx->promise, result);
    
    free(ctx);
    return NULL;
}

/* Helper function for async call */
zn_promise_t *_zn_async_call(void *(*func)(void *), void *args) {
    if (!func) {
        return zn_promise_reject(NULL);
    }
    
    zn_promise_t *promise = (zn_promise_t *)malloc(sizeof(zn_promise_t));
    if (!promise) {
        return NULL;
    }
    
    memset(promise, 0, sizeof(zn_promise_t));
    promise->state = ZN_PROMISE_PENDING;
    
    if (zn_mutex_init(&promise->mutex) != 0 || 
        zn_cond_init(&promise->cond) != 0) {
        free(promise);
        return NULL;
    }
    
    /* Create context for thread */
    async_call_ctx_t *ctx = (async_call_ctx_t *)malloc(sizeof(async_call_ctx_t));
    if (!ctx) {
        zn_mutex_destroy(&promise->mutex);
        zn_cond_destroy(&promise->cond);
        free(promise);
        return NULL;
    }
    
    ctx->func = func;
    ctx->args = args;
    ctx->promise = promise;
    
    /* Create and start thread */
    if (zn_thread_init(&promise->thread) != 0 ||
        zn_thread_create(&promise->thread, async_thread_func, ctx) != 0) {
        zn_mutex_destroy(&promise->mutex);
        zn_cond_destroy(&promise->cond);
        free(ctx);
        free(promise);
        return NULL;
    }
    
    promise->has_thread = true;
    
    return promise;
}
