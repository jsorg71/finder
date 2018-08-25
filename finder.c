
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>

#include "finder.h"
#include "finder_gui.h"
#include "finder_list.h"
#include "finder_thread.h"
#include "finder_event.h"

/* main to work */
#define FINDER_CMD_START        1
/* work to main */
#define FINDER_CMD_DONE         101
#define FINDER_CMD_ADD_ONE      102

struct work_item
{
    int cmd;
    int pad0;
    char* filename;
};

/*****************************************************************************/
/* main thread */
int
gui_init(struct finder_info* fi)
{
    writeln(fi, "gui_init");
    if (finder_mutex_create(&(fi->list_mutex)) != 0)
    {
        writeln(fi, "finder_mutex_create failed");
        return 1;
    }
    if (finder_event_create(&(fi->work_term_event)) != 0)
    {
        writeln(fi, "finder_event_create failed");
        finder_mutex_delete(fi->list_mutex);
        return 2;
    }
    if (finder_event_create(&(fi->main_to_work_event)) != 0)
    {
        writeln(fi, "finder_event_create failed");
        finder_mutex_delete(fi->list_mutex);
        finder_event_delete(fi->work_term_event);
        return 2;
    }
    if (finder_list_create(0, 0, &(fi->main_to_work_list)) != 0)
    {
        writeln(fi, "finder_list_create failed");
        finder_mutex_delete(fi->list_mutex);
        finder_event_delete(fi->work_term_event);
        finder_event_delete(fi->main_to_work_event);
        return 3;
    }
    if (finder_list_create(0, 0, &(fi->work_to_main_list)) != 0)
    {
        writeln(fi, "finder_list_create failed");
        finder_mutex_delete(fi->list_mutex);
        finder_event_delete(fi->work_term_event);
        finder_event_delete(fi->main_to_work_event);
        finder_list_delete(fi->main_to_work_list);
        return 4;
    }
    return 0;
}

/*****************************************************************************/
/* main thread */
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
    vsnprintf(log_line, 1024, format, ap);
    va_end(ap);
    printf("%s\n", log_line);
    free(log_line);
    return 0;
}

/*****************************************************************************/
/* work thread */
static int
process_work_item(struct finder_info* fi, struct work_item* wi)
{
    struct work_item* lwi;
    int rv;

    //writeln(fi, "process_work_item");
    if (wi == NULL)
    {
        return 0;
    }
    rv = 0;
    if (wi->cmd == FINDER_CMD_START)
    {

        DIR *d;
        struct dirent *dir;
        //d = opendir(".");
        d = opendir(fi->look_in);
        if (d != NULL)
        {
            while ((dir = readdir(d)) != NULL)
            {
                //printf("%s\n", dir->d_name);
                if ((dir->d_name[0] != '.') || (fi->show_hidden != 0))
                {
                    lwi = (struct work_item*)calloc(1, sizeof(struct work_item));
                    if (lwi != NULL)
                    {
                        lwi->cmd = FINDER_CMD_ADD_ONE;
                        lwi->filename = strdup(dir->d_name);
                        finder_mutex_lock(fi->list_mutex);
                        finder_list_add_item(fi->work_to_main_list, (ITYPE)lwi);
                        finder_mutex_unlock(fi->list_mutex);
                        gui_set_event(fi);
                    }
                }
            }
            closedir(d);
        }
        rv = 1;

    }
    //writeln(fi, "process_work_item: free item");
    free(wi);
    return rv;
}

/*****************************************************************************/
/* work thread */
static FINDER_THREAD_RV FINDER_THREAD_CC
finder_thread(void* arg)
{
    int cont;
    int error;
    int list_count;
    int max_fd;
    int work_term_fd;
    int main_to_work_fd;
    fd_set rfds;
    struct finder_info* fi;
    struct work_item* wi;

    fi = (struct finder_info*)arg;
    writeln(fi, "thread start");
    cont = 1;
    while (cont)
    {
        //writeln(fi, "loop");
        FD_ZERO(&rfds);
        work_term_fd = finder_event_get_wait_obj(fi->work_term_event);
        FD_SET((unsigned int)work_term_fd, &rfds);
        main_to_work_fd = finder_event_get_wait_obj(fi->main_to_work_event);
        FD_SET((unsigned int)main_to_work_fd, &rfds);
        max_fd = work_term_fd;
        if (main_to_work_fd > max_fd)
        {
            max_fd = main_to_work_fd;
        }
        error = select(max_fd + 1, &rfds, 0, 0, 0);
        if (error < 0)
        {
            writeln(fi, "select failed");
        }
        if (finder_event_is_set(fi->work_term_event))
        {
            finder_event_clear(fi->work_term_event);
            cont = 0;
        }
        if (finder_event_is_set(fi->main_to_work_event))
        {
            finder_event_clear(fi->main_to_work_event);
            while (1)
            {
                finder_mutex_lock(fi->list_mutex);
                list_count = finder_list_get_count(fi->main_to_work_list);
                if (list_count > 0)
                {
                    wi = (struct work_item*)finder_list_get_item(fi->main_to_work_list, 0);
                    finder_list_remove_item(fi->main_to_work_list, 0);
                    finder_mutex_unlock(fi->list_mutex);
                    if (process_work_item(fi, wi) != 0)
                    {
                        cont = 0;
                    }
                }
                else
                {
                    finder_mutex_unlock(fi->list_mutex);
                    break;
                }
            }
        }
    }
    wi = (struct work_item*)calloc(1, sizeof(struct work_item));
    if (wi != NULL)
    {
        wi->cmd = FINDER_CMD_DONE;
        finder_mutex_lock(fi->list_mutex);
        finder_list_add_item(fi->work_to_main_list, (ITYPE)wi);
        finder_mutex_unlock(fi->list_mutex);
        gui_set_event(fi);
    }
    writeln(fi, "thread stop");
    return 0;
}

/*****************************************************************************/
/* main thread */
int
start_find(struct finder_info* fi)
{
    struct work_item* wi;

    writeln(fi, "start_find:");
    writeln(fi, "  named [%s]", fi->named);
    writeln(fi, "  look_in [%s]", fi->look_in);
    writeln(fi, "  include_subfolders %d", fi->include_subfolders);
    writeln(fi, "  case_sensitive %d", fi->case_sensitive);
    writeln(fi, "  show_hidden %d", fi->show_hidden);

    finder_mutex_lock(fi->list_mutex);
    finder_list_clear(fi->main_to_work_list, 0, 0);
    finder_list_clear(fi->work_to_main_list, 0, 0);
    finder_mutex_unlock(fi->list_mutex);
    finder_event_clear(fi->work_term_event);
    finder_event_clear(fi->main_to_work_event);

    finder_thread_create(finder_thread, fi);

    wi = (struct work_item*)calloc(1, sizeof(struct work_item));
    if (wi != NULL)
    {
        wi->cmd = FINDER_CMD_START;
        finder_mutex_lock(fi->list_mutex);
        finder_list_add_item(fi->main_to_work_list, (ITYPE)wi);
        finder_mutex_unlock(fi->list_mutex);
        finder_event_set(fi->main_to_work_event);
    }

    return 0;
}

/*****************************************************************************/
/* main thread */
int
stop_find(struct finder_info* fi)
{
    writeln(fi, "stop_find:");
    finder_event_set(fi->work_term_event);
    return 0;
}

/*****************************************************************************/
/* main thread */
static int
main_process_work_item(struct finder_info* fi, struct work_item* wi)
{
    //writeln(fi, "main_process_work_item");
    if (wi == NULL)
    {
        return 0;
    }
    //writeln(fi, "main_process_work_item: cmd %d", wi->cmd);
    if (wi->cmd == FINDER_CMD_DONE)
    {
        gui_find_done(fi);
    }
    else if (wi->cmd == FINDER_CMD_ADD_ONE)
    {
        gui_add_one(fi, wi->filename);
        free(wi->filename);
    }
    //writeln(fi, "main_process_work_item: free item");
    free(wi);
    return 0;
}

/*****************************************************************************/
/* main thread */
int
event_callback(struct finder_info* fi)
{
    int list_count;
    struct work_item* wi;

    //writeln(fi, "event_callback:");
    while (1)
    {
        finder_mutex_lock(fi->list_mutex);
        list_count = finder_list_get_count(fi->work_to_main_list);
        if (list_count > 0)
        {
            wi = (struct work_item*)finder_list_get_item(fi->work_to_main_list, 0);
            finder_list_remove_item(fi->work_to_main_list, 0);
            finder_mutex_unlock(fi->list_mutex);
            main_process_work_item(fi, wi);
        }
        else
        {
            finder_mutex_unlock(fi->list_mutex);
            break;
        }
    }
    return 0;
}

