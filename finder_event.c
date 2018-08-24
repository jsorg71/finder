
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "finder_event.h"

struct finder_event
{
    int pipe[2];
};

/*****************************************************************************/
int
finder_event_create(void** event1)
{
    struct finder_event* levent;

    levent = (struct finder_event*)calloc(1, sizeof(struct finder_event));
    if (levent == NULL)
    {
        return 1;
    }
    pipe(levent->pipe);
    *event1 = levent;
    return 0;
}

/*****************************************************************************/
int
finder_event_delete(void* event1)
{
    struct finder_event* levent;

    levent = (struct finder_event*)event1;
    if (levent == NULL)
    {
        return 0;
    }
    close(levent->pipe[0]);
    close(levent->pipe[1]);
    free(levent);
    return 0;
}

/*****************************************************************************/
int
finder_event_set(void* event1)
{
    struct finder_event* levent;

    levent = (struct finder_event*)event1;
    if (!finder_event_is_set(levent))
    {
        if (write(levent->pipe[1], "sig", 4) != 4)
        {
            return 1;
        }
    }
    return 0;
}

/*****************************************************************************/
int
finder_event_clear(void* event1)
{
    struct finder_event* levent;
    char val[4];

    levent = (struct finder_event*)event1;
    while (finder_event_is_set(levent))
    {
        if (read(levent->pipe[0], val, 4) < 1)
        {
            return 1;
        }
    }
    return 0;
}

/*****************************************************************************/
int
finder_event_is_set(void* event1)
{
    struct finder_event* levent;
    struct timeval time;
    fd_set rfds;
    int rv;
    int sck;

    levent = (struct finder_event*)event1;
    memset(&time, 0, sizeof(time));
    sck = levent->pipe[0];
    FD_ZERO(&rfds);
    FD_SET(((unsigned int)sck), &rfds);
    rv = select(sck + 1, &rfds, 0, 0, &time);
    if (rv > 0)
    {
        return 1;
    }
    return 0;
}

/*****************************************************************************/
FINDER_WAIT_OBJ
finder_event_get_wait_obj(void* event1)
{
    struct finder_event* levent;

    levent = (struct finder_event*)event1;
    return levent->pipe[0];
}

