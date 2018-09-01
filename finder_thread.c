
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "finder_thread.h"

/*****************************************************************************/
int
finder_thread_create(start_routine_proc start_routine, void* arg)
{
    int rv;
    pthread_t thread;

    memset(&thread, 0, sizeof(pthread_t));
    /* pthread_create returns error */
    rv = pthread_create(&thread, NULL, start_routine, arg);
    if (rv == 0)
    {
        rv = pthread_detach(thread);
    }
    return rv;
}

/*****************************************************************************/
int
finder_mutex_create(void** mutex)
{
    pthread_mutex_t* lmutex;

    lmutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
    if (lmutex == NULL)
    {
        return 1;
    }
    pthread_mutex_init(lmutex, NULL);
    *mutex = lmutex;
    return 0;
}

/*****************************************************************************/
int
finder_mutex_delete(void* mutex)
{
    pthread_mutex_t* lmutex;

    lmutex = (pthread_mutex_t*)mutex;
    pthread_mutex_destroy(lmutex);
    free(lmutex);
    return 0;
}

/*****************************************************************************/
int
finder_mutex_lock(void* mutex)
{
    pthread_mutex_lock((pthread_mutex_t*)mutex);
    return 0;
}

/*****************************************************************************/
int
finder_mutex_unlock(void* mutex)
{
    pthread_mutex_unlock((pthread_mutex_t*)mutex);
    return 0;
}
