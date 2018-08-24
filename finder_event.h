
#if !defined(FINDER_EVENT_H)
#define FINDER_EVENT_H

#define FINDER_WAIT_OBJ int

#ifdef __cplusplus
extern "C" {
#endif

int
finder_event_create(void** event1);
int
finder_event_delete(void* event1);
int
finder_event_set(void* event1);
int
finder_event_clear(void* event1);
int
finder_event_is_set(void* event1);
FINDER_WAIT_OBJ
finder_event_get_wait_obj(void* event1);

#ifdef __cplusplus
}
#endif

#endif

