/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2018-2019
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

#if !defined(FINDER_THREAD_H)
#define FINDER_THREAD_H

#if defined(_WIN32)
#define FINDER_THREAD_RV unsigned long
#define FINDER_THREAD_CC __stdcall
#else
#define FINDER_THREAD_RV void*
#define FINDER_THREAD_CC
#endif

typedef FINDER_THREAD_RV (FINDER_THREAD_CC * start_routine_proc)(void*);

#ifdef __cplusplus
extern "C" {
#endif

int
finder_thread_create(start_routine_proc start_routine, void* arg);
int
finder_mutex_create(void** mutex);
int
finder_mutex_delete(void* mutex);
int
finder_mutex_lock(void* mutex);
int
finder_mutex_unlock(void* mutex);

#ifdef __cplusplus
}
#endif

#endif

