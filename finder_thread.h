
#if !defined(FINDER_THREAD_H)
#define FINDER_THREAD_H

#define FINDER_THREAD_RV void*
#define FINDER_THREAD_CC

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

