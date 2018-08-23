
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "finder_event.h"

/*****************************************************************************/
int
finder_event_init(struct finder_event* event1)
{
    pipe(event1->pipe);
    return 0;
}

/*****************************************************************************/
int
finder_event_deinit(struct finder_event* event1)
{
    close(event1->pipe[0]);
    close(event1->pipe[1]);
    return 0;
}

/*****************************************************************************/
int
finder_event_set(struct finder_event* event1)
{
    if (!finder_event_is_set(event1))
    {
        if (write(event1->pipe[1], "sig", 4) != 4)
        {
            return 1;
        }
    }
    return 0;
}

/*****************************************************************************/
int
finder_event_clear(struct finder_event* event1)
{
    char val[4];

    while (finder_event_is_set(event1))
    {
        if (read(event1->pipe[0], val, 4) < 1)
        {
            return 1;
        }
    }
    return 0;
}

/*****************************************************************************/
int
finder_event_is_set(struct finder_event* event1)
{
    struct timeval time;
    fd_set rfds;
    int rv;
    int sck;

    memset(&time, 0, sizeof(time));
    sck = event1->pipe[0];
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
finder_event_get_wait_obj(struct finder_event* event1)
{
    return event1->pipe[0];
}

