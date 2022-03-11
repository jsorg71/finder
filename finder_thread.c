/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2018-2022
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "finder_thread.h"

/*****************************************************************************/
int
finder_thread_create(start_routine_proc start_routine, void* arg)
{
#if defined(_WIN32)
    int rv;
    DWORD thread_id;
    HANDLE thread;

    /* CreateThread returns handle or zero on error */
    thread = CreateThread(0, 0, start_routine, arg, 0, &thread_id);
    rv = !thread;
    CloseHandle(thread);
    return rv;
#else
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
#endif
}

/*****************************************************************************/
int
finder_mutex_create(void** mutex)
{
#if defined(_WIN32)
    HANDLE lmutex;

    lmutex = CreateMutex(NULL, FALSE, NULL);
    if (lmutex == NULL)
    {
        return 1;
    }
    *mutex = lmutex;
    return 0;
#else
    pthread_mutex_t* lmutex;

    lmutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
    if (lmutex == NULL)
    {
        return 1;
    }
    pthread_mutex_init(lmutex, NULL);
    *mutex = lmutex;
    return 0;
#endif
}

/*****************************************************************************/
int
finder_mutex_delete(void* mutex)
{
#if defined(_WIN32)
    CloseHandle(mutex);
    return 0;
#else
    pthread_mutex_t* lmutex;

    lmutex = (pthread_mutex_t*)mutex;
    pthread_mutex_destroy(lmutex);
    free(lmutex);
    return 0;
#endif
}

/*****************************************************************************/
int
finder_mutex_lock(void* mutex)
{
#if defined(_WIN32)
    WaitForSingleObject(mutex, INFINITE);
    return 0;
#else
    pthread_mutex_lock((pthread_mutex_t*)mutex);
    return 0;
#endif
}

/*****************************************************************************/
int
finder_mutex_unlock(void* mutex)
{
#if defined(_WIN32)
    ReleaseMutex(mutex);
    return 0;
#else
    pthread_mutex_unlock((pthread_mutex_t*)mutex);
    return 0;
#endif
}

