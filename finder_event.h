
#if !defined(FINDER_EVENT_H)
#define FINDER_EVENT_H

#define FINDER_WAIT_OBJ int

struct finder_event
{
    int pipe[2];
};

#ifdef __cplusplus
extern "C" {
#endif

int
finder_event_init(struct finder_event* event1);
int
finder_event_deinit(struct finder_event* event1);
int
finder_event_set(struct finder_event* event1);
int
finder_event_clear(struct finder_event* event1);
int
finder_event_is_set(struct finder_event* event1);
FINDER_WAIT_OBJ
finder_event_get_wait_obj(struct finder_event* event1);

#ifdef __cplusplus
}
#endif

#endif

