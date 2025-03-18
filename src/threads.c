/**
 * @file threads.c
 * @brief Implementation of the threads wrapper
 */

#include "threads.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Thread pool implementation */
struct task_node {
    zn_task_func_t func;
    void *arg;
    struct task_node *next;
};

struct zn_thread_pool {
    zn_thread_t *threads;
    size_t num_threads;
    struct task_node *task_queue;
    struct task_node *task_queue_tail;
    zn_mutex_t queue_mutex;
    zn_cond_t queue_cond;
    bool shutdown;
};

/* Thread pool worker function */
static void *thread_pool_worker(void *arg) {
    zn_thread_pool_t *pool = (zn_thread_pool_t *)arg;
    
    while (1) {
        zn_task_func_t func = NULL;
        void *task_arg = NULL;
        
        zn_mutex_lock(&pool->queue_mutex);
        
        while (pool->task_queue == NULL && !pool->shutdown) {
            zn_cond_wait(&pool->queue_cond, &pool->queue_mutex);
        }
        
        if (pool->shutdown && pool->task_queue == NULL) {
            zn_mutex_unlock(&pool->queue_mutex);
            break;
        }
        
        struct task_node *task = pool->task_queue;
        if (task) {
            pool->task_queue = task->next;
            if (pool->task_queue == NULL) {
                pool->task_queue_tail = NULL;
            }
            
            func = task->func;
            task_arg = task->arg;
            free(task);
        }
        
        zn_mutex_unlock(&pool->queue_mutex);
        
        if (func) {
            func(task_arg);
        }
    }
    
    return NULL;
}

int zn_thread_init(zn_thread_t *thread) {
    if (!thread) {
        return -1;
    }
    
    memset(thread, 0, sizeof(zn_thread_t));
    thread->is_running = false;
    
    return 0;
}

int zn_thread_create(zn_thread_t *thread, void *(*function)(void *), void *arg) {
    if (!thread || !function) {
        return -1;
    }
    
    thread->function = function;
    thread->arg = arg;
    
    int result = pthread_create(&thread->thread, NULL, function, arg);
    if (result == 0) {
        thread->is_running = true;
    }
    
    return result;
}

int zn_thread_join(zn_thread_t *thread, void **result) {
    if (!thread || !thread->is_running) {
        return -1;
    }
    
    int ret = pthread_join(thread->thread, result ? result : &thread->result);
    if (ret == 0) {
        thread->is_running = false;
    }
    
    return ret;
}

int zn_thread_detach(zn_thread_t *thread) {
    if (!thread || !thread->is_running) {
        return -1;
    }
    
    int ret = pthread_detach(thread->thread);
    if (ret == 0) {
        thread->is_running = false;
    }
    
    return ret;
}

void zn_thread_destroy(zn_thread_t *thread) {
    if (!thread) {
        return;
    }
    
    if (thread->is_running) {
        zn_thread_join(thread, NULL);
    }
    
    memset(thread, 0, sizeof(zn_thread_t));
}

int zn_mutex_init(zn_mutex_t *mutex) {
    if (!mutex) {
        return -1;
    }
    
    int result = pthread_mutex_init(&mutex->mutex, NULL);
    if (result == 0) {
        mutex->initialized = true;
    }
    
    return result;
}

int zn_mutex_lock(zn_mutex_t *mutex) {
    if (!mutex || !mutex->initialized) {
        return -1;
    }
    
    return pthread_mutex_lock(&mutex->mutex);
}

int zn_mutex_trylock(zn_mutex_t *mutex) {
    if (!mutex || !mutex->initialized) {
        return -1;
    }
    
    return pthread_mutex_trylock(&mutex->mutex);
}

int zn_mutex_unlock(zn_mutex_t *mutex) {
    if (!mutex || !mutex->initialized) {
        return -1;
    }
    
    return pthread_mutex_unlock(&mutex->mutex);
}

int zn_mutex_destroy(zn_mutex_t *mutex) {
    if (!mutex || !mutex->initialized) {
        return -1;
    }
    
    int result = pthread_mutex_destroy(&mutex->mutex);
    if (result == 0) {
        mutex->initialized = false;
    }
    
    return result;
}

int zn_cond_init(zn_cond_t *cond) {
    if (!cond) {
        return -1;
    }
    
    int result = pthread_cond_init(&cond->cond, NULL);
    if (result == 0) {
        cond->initialized = true;
    }
    
    return result;
}

int zn_cond_wait(zn_cond_t *cond, zn_mutex_t *mutex) {
    if (!cond || !mutex || !cond->initialized || !mutex->initialized) {
        return -1;
    }
    
    return pthread_cond_wait(&cond->cond, &mutex->mutex);
}

int zn_cond_signal(zn_cond_t *cond) {
    if (!cond || !cond->initialized) {
        return -1;
    }
    
    return pthread_cond_signal(&cond->cond);
}

int zn_cond_broadcast(zn_cond_t *cond) {
    if (!cond || !cond->initialized) {
        return -1;
    }
    
    return pthread_cond_broadcast(&cond->cond);
}

int zn_cond_destroy(zn_cond_t *cond) {
    if (!cond || !cond->initialized) {
        return -1;
    }
    
    int result = pthread_cond_destroy(&cond->cond);
    if (result == 0) {
        cond->initialized = false;
    }
    
    return result;
}

zn_thread_pool_t *zn_thread_pool_create(size_t num_threads) {
    if (num_threads == 0) {
        num_threads = zn_get_num_cores();
    }
    
    zn_thread_pool_t *pool = (zn_thread_pool_t *)malloc(sizeof(zn_thread_pool_t));
    if (!pool) {
        return NULL;
    }
    
    memset(pool, 0, sizeof(zn_thread_pool_t));
    
    pool->threads = (zn_thread_t *)malloc(num_threads * sizeof(zn_thread_t));
    if (!pool->threads) {
        free(pool);
        return NULL;
    }
    
    pool->num_threads = num_threads;
    pool->task_queue = NULL;
    pool->task_queue_tail = NULL;
    pool->shutdown = false;
    
    if (zn_mutex_init(&pool->queue_mutex) != 0) {
        free(pool->threads);
        free(pool);
        return NULL;
    }
    
    if (zn_cond_init(&pool->queue_cond) != 0) {
        zn_mutex_destroy(&pool->queue_mutex);
        free(pool->threads);
        free(pool);
        return NULL;
    }
    
    /* Create worker threads */
    for (size_t i = 0; i < num_threads; i++) {
        zn_thread_init(&pool->threads[i]);
        if (zn_thread_create(&pool->threads[i], thread_pool_worker, pool) != 0) {
            pool->shutdown = true;
            zn_cond_broadcast(&pool->queue_cond);
            
            /* Join any threads that were created successfully */
            for (size_t j = 0; j < i; j++) {
                zn_thread_join(&pool->threads[j], NULL);
            }
            
            /* Clean up resources */
            zn_cond_destroy(&pool->queue_cond);
            zn_mutex_destroy(&pool->queue_mutex);
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }
    
    return pool;
}

int zn_thread_pool_add_task(zn_thread_pool_t *pool, zn_task_func_t func, void *arg) {
    if (!pool || !func) {
        return -1;
    }
    
    struct task_node *task = (struct task_node *)malloc(sizeof(struct task_node));
    if (!task) {
        return -1;
    }
    
    task->func = func;
    task->arg = arg;
    task->next = NULL;
    
    zn_mutex_lock(&pool->queue_mutex);
    
    if (pool->shutdown) {
        zn_mutex_unlock(&pool->queue_mutex);
        free(task);
        return -1;
    }
    
    /* Add task to queue */
    if (pool->task_queue == NULL) {
        pool->task_queue = task;
        pool->task_queue_tail = task;
    } else {
        pool->task_queue_tail->next = task;
        pool->task_queue_tail = task;
    }
    
    /* Signal a worker thread */
    zn_cond_signal(&pool->queue_cond);
    
    zn_mutex_unlock(&pool->queue_mutex);
    
    return 0;
}

void zn_thread_pool_destroy(zn_thread_pool_t *pool) {
    if (!pool) {
        return;
    }
    
    zn_mutex_lock(&pool->queue_mutex);
    
    /* Clean up task queue */
    struct task_node *task, *next_task;
    for (task = pool->task_queue; task != NULL; task = next_task) {
        next_task = task->next;
        free(task);
    }
    
    pool->task_queue = NULL;
    pool->task_queue_tail = NULL;
    pool->shutdown = true;
    
    zn_cond_broadcast(&pool->queue_cond);
    zn_mutex_unlock(&pool->queue_mutex);
    
    /* Wait for worker threads to exit */
    for (size_t i = 0; i < pool->num_threads; i++) {
        zn_thread_join(&pool->threads[i], NULL);
    }
    
    /* Clean up resources */
    zn_cond_destroy(&pool->queue_cond);
    zn_mutex_destroy(&pool->queue_mutex);
    free(pool->threads);
    free(pool);
}

size_t zn_get_num_cores(void) {
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    return (num_cores > 0) ? (size_t)num_cores : 1;
}

pthread_t zn_thread_self(void) {
    return pthread_self();
}

/* Thread-specific data key */
static pthread_key_t thread_data_key;
static bool thread_key_initialized = false;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

/* Initialize thread-specific data key */
static void init_thread_key(void) {
    pthread_key_create(&thread_data_key, NULL);
    thread_key_initialized = true;
}

int zn_thread_set_data(zn_thread_t *thread, void *data) {
    if (!thread) {
        return -1;
    }
    
    /* Initialize key if needed */
    pthread_once(&key_once, init_thread_key);
    
    /* Store data in thread-local storage */
    pthread_setspecific(thread_data_key, data);
    
    return 0;
}

void *zn_thread_self_data(void) {
    /* Initialize key if needed */
    pthread_once(&key_once, init_thread_key);
    
    /* Retrieve data from thread-local storage */
    return pthread_getspecific(thread_data_key);
}
