
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "finder.h"
#include "finder_gui.h"
#include "finder_list.h"
#include "finder_thread.h"
#include "finder_event.h"

/*****************************************************************************/
int
gui_init(struct finder_info* fi)
{
    writeln(fi, "gui_init");
    finder_mutex_create(&(fi->list_mutex));
    finder_event_create(&(fi->work_term_event));
    fi->main_to_work_list = finder_list_create();
    fi->work_to_main_list = finder_list_create();
    return 0;
}

/*****************************************************************************/
int
gui_deinit(struct finder_info* fi)
{
    writeln(fi, "gui_deinit");
    finder_mutex_delete(fi->list_mutex);
    finder_event_delete(fi->work_term_event);
    finder_list_delete(fi->main_to_work_list);
    finder_list_delete(fi->work_to_main_list);
    return 0;
}

/*****************************************************************************/
int
writeln(struct finder_info* fi, const char* format, ...)
{
    va_list ap;
    char* log_line;

    log_line = (char*)malloc(1024);
    va_start(ap, format);
    vsnprintf(log_line, 1024 - 2, format, ap);
    va_end(ap);
    printf("%s\n", log_line);
    free(log_line);
    return 0;
}

/*****************************************************************************/
static FINDER_THREAD_RV FINDER_THREAD_CC
finder_thread(void* arg)
{
    int cont;
    int fd;
    int error;
    struct finder_info* fi;
    fd_set rfds;

    fi = (struct finder_info*)arg;
    writeln(fi, "thread start");
    cont = 1;
    while (cont)
    {
        writeln(fi, "loop");
        FD_ZERO(&rfds);
        fd = finder_event_get_wait_obj(fi->work_term_event);
        FD_SET((unsigned int)fd, &rfds);
        error = select(fd + 1, &rfds, 0, 0, 0);
        if (error > 0)
        {
            if (FD_ISSET(((unsigned int)fd), &rfds))
            {
                cont = 0;
                finder_event_clear(fi->work_term_event);
            }
        }
        gui_set_event(fi);
    }
    writeln(fi, "thread shutting down");
    gui_set_event(fi);
    return 0; 
}

/*****************************************************************************/
int
start_find(struct finder_info* fi)
{
    writeln(fi, "start_find:");
    writeln(fi, "  named [%s]", fi->named);
    writeln(fi, "  look_in [%s]", fi->look_in);
    writeln(fi, "  include_subfolders %d", fi->include_subfolders);
    writeln(fi, "  case_sensitive %d", fi->case_sensitive);
    writeln(fi, "  show_hidden %d", fi->show_hidden);
    //gui_set_event(fi);
    finder_thread_create(finder_thread, fi);
    return 0;
}

/*****************************************************************************/
int
stop_find(struct finder_info* fi)
{
    printf("stop_find:\n");
    finder_event_set(fi->work_term_event);
    return 0; 
}

/*****************************************************************************/
int
event_callback(struct finder_info* fi)
{
    printf("event_callback:\n");
    return 0;
}

