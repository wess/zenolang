/**
 * @file threads.h
 * @brief A simple wrapper around pthread to simplify thread usage
 */

#ifndef ZENO_THREADS_H
#define ZENO_THREADS_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Thread handle structure
 */
typedef struct zn_thread {
    pthread_t thread;
    bool is_running;
    void *(*function)(void *);
    void *arg;
    void *result;
} zn_thread_t;

/**
 * @brief Mutex structure
 */
typedef struct zn_mutex {
    pthread_mutex_t mutex;
    bool initialized;
} zn_mutex_t;

/**
 * @brief Condition variable structure
 */
typedef struct zn_cond {
    pthread_cond_t cond;
    bool initialized;
} zn_cond_t;

/**
 * @brief Thread pool structure
 */
typedef struct zn_thread_pool zn_thread_pool_t;

/**
 * @brief Task function type
 */
typedef void (*zn_task_func_t)(void *);

/**
 * @brief Initialize a thread
 * @param thread Pointer to a thread handle
 * @return 0 on success, error code otherwise
 */
int zn_thread_init(zn_thread_t *thread);

/**
 * @brief Create and start a new thread
 * @param thread Pointer to a thread handle
 * @param function Thread function to execute
 * @param arg Argument to pass to the function
 * @return 0 on success, error code otherwise
 */
int zn_thread_create(zn_thread_t *thread, void *(*function)(void *), void *arg);

/**
 * @brief Wait for a thread to complete
 * @param thread Pointer to a thread handle
 * @param result Pointer to store the thread result
 * @return 0 on success, error code otherwise
 */
int zn_thread_join(zn_thread_t *thread, void **result);

/**
 * @brief Detach a thread
 * @param thread Pointer to a thread handle
 * @return 0 on success, error code otherwise
 */
int zn_thread_detach(zn_thread_t *thread);

/**
 * @brief Clean up thread resources
 * @param thread Pointer to a thread handle
 */
void zn_thread_destroy(zn_thread_t *thread);

/**
 * @brief Initialize a mutex
 * @param mutex Pointer to a mutex
 * @return 0 on success, error code otherwise
 */
int zn_mutex_init(zn_mutex_t *mutex);

/**
 * @brief Lock a mutex
 * @param mutex Pointer to a mutex
 * @return 0 on success, error code otherwise
 */
int zn_mutex_lock(zn_mutex_t *mutex);

/**
 * @brief Try to lock a mutex (non-blocking)
 * @param mutex Pointer to a mutex
 * @return 0 on success, error code otherwise
 */
int zn_mutex_trylock(zn_mutex_t *mutex);

/**
 * @brief Unlock a mutex
 * @param mutex Pointer to a mutex
 * @return 0 on success, error code otherwise
 */
int zn_mutex_unlock(zn_mutex_t *mutex);

/**
 * @brief Clean up mutex resources
 * @param mutex Pointer to a mutex
 * @return 0 on success, error code otherwise
 */
int zn_mutex_destroy(zn_mutex_t *mutex);

/**
 * @brief Initialize a condition variable
 * @param cond Pointer to a condition variable
 * @return 0 on success, error code otherwise
 */
int zn_cond_init(zn_cond_t *cond);

/**
 * @brief Wait on a condition variable
 * @param cond Pointer to a condition variable
 * @param mutex Pointer to a mutex
 * @return 0 on success, error code otherwise
 */
int zn_cond_wait(zn_cond_t *cond, zn_mutex_t *mutex);

/**
 * @brief Signal a condition variable (wake one thread)
 * @param cond Pointer to a condition variable
 * @return 0 on success, error code otherwise
 */
int zn_cond_signal(zn_cond_t *cond);

/**
 * @brief Broadcast a condition variable (wake all threads)
 * @param cond Pointer to a condition variable
 * @return 0 on success, error code otherwise
 */
int zn_cond_broadcast(zn_cond_t *cond);

/**
 * @brief Clean up condition variable resources
 * @param cond Pointer to a condition variable
 * @return 0 on success, error code otherwise
 */
int zn_cond_destroy(zn_cond_t *cond);

/**
 * @brief Create a thread pool
 * @param num_threads Number of worker threads to create
 * @return Pointer to the thread pool or NULL on error
 */
zn_thread_pool_t *zn_thread_pool_create(size_t num_threads);

/**
 * @brief Add a task to the thread pool
 * @param pool Thread pool
 * @param func Task function
 * @param arg Argument to pass to the function
 * @return 0 on success, error code otherwise
 */
int zn_thread_pool_add_task(zn_thread_pool_t *pool, zn_task_func_t func, void *arg);

/**
 * @brief Wait for all tasks to complete and destroy the thread pool
 * @param pool Thread pool
 */
void zn_thread_pool_destroy(zn_thread_pool_t *pool);

/**
 * @brief Get the number of CPU cores available
 * @return Number of cores
 */
size_t zn_get_num_cores(void);

/**
 * @brief Get the current thread ID
 * @return Thread ID
 */
pthread_t zn_thread_self(void);

/**
 * @brief Set thread-specific data
 * @param thread Thread handle
 * @param data Data to store
 * @return 0 on success, error code otherwise
 */
int zn_thread_set_data(zn_thread_t *thread, void *data);

/**
 * @brief Get thread-specific data
 * @return Thread-specific data or NULL if not set
 */
void *zn_thread_self_data(void);

#endif /* ZENO_THREADS_H */
