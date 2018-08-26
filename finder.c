
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
    char* in_subfolder;
    char* size;
    char* modified;
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
    if (finder_list_create(1024, 1024, &(fi->main_to_work_list)) != 0)
    {
        writeln(fi, "finder_list_create failed");
        finder_mutex_delete(fi->list_mutex);
        finder_event_delete(fi->work_term_event);
        finder_event_delete(fi->main_to_work_event);
        return 3;
    }
    if (finder_list_create(1024, 1024, &(fi->work_to_main_list)) != 0)
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
static void
listdir(struct finder_info* fi, struct work_item* wi, const char* name)
{
    DIR* dir;
    struct dirent* entry;
    struct work_item* lwi;
    int count;
    int look_in_bytes;
    int name_bytes;
    int look_in_text_alloc_bytes;
    char* path;
    char* look_in_text;

    look_in_bytes = strlen(fi->look_in);
    name_bytes = strlen(name);
    //writeln(fi, "%d %d", look_in_bytes, name_bytes);
    if (name_bytes < look_in_bytes)
    {
        writeln(fi, "%s", "error");
        return;
    }

    dir = opendir(name);
    if (dir == NULL)
    {
        writeln(fi, "%s", "error");
        return;
    }

    look_in_text_alloc_bytes = name_bytes - look_in_bytes + 1;
    look_in_text = (char *)calloc(look_in_text_alloc_bytes, 1);
    if (name_bytes > look_in_bytes)
    {
        snprintf(look_in_text, look_in_text_alloc_bytes, "%s", name + look_in_bytes + 1);
    }

    count = 0;
    while (1)
    {
        entry = readdir(dir);
        if (entry == NULL)
        {
            break;
        }
        if (entry->d_type == DT_DIR)
        {
            if ((strcmp(entry->d_name, ".") == 0) ||
                (strcmp(entry->d_name, "..") == 0))
            {
                continue;
            }
            if (fi->include_subfolders)
            {
                path = (char*)malloc(4096);
                snprintf(path, 4096, "%s/%s", name, entry->d_name);
                //writeln(fi, "%s", path);
                listdir(fi, wi, path);
                free(path);
            }
        }
        else
        {
            if ((entry->d_name[0] == '.') && (fi->show_hidden == 0))
            {
                continue;
            }
            lwi = (struct work_item *)calloc(1, sizeof(struct work_item));
            if (lwi != NULL)
            {
                lwi->cmd = FINDER_CMD_ADD_ONE;
                lwi->filename = strdup(entry->d_name);
                lwi->in_subfolder = strdup(look_in_text);
                finder_mutex_lock(fi->list_mutex);
                finder_list_add_item(fi->work_to_main_list, (ITYPE)lwi);
                finder_mutex_unlock(fi->list_mutex);
            }
            count++;
            if ((count % 1024) == 0)
            {
                gui_set_event(fi);
                if (finder_event_is_set(fi->work_term_event))
                {
                    break;
                }
            }
        }
    }
    free(look_in_text);
    closedir(dir);
    if (count > 0)
    {
        gui_set_event(fi);
    }
}

/*****************************************************************************/
/* work thread */
static int
process_work_item(struct finder_info* fi, struct work_item* wi)
{
    int rv;

    //writeln(fi, "process_work_item");
    if (wi == NULL)
    {
        return 0;
    }
    rv = 0;
    if (wi->cmd == FINDER_CMD_START)
    {
        listdir(fi, wi, fi->look_in);
        rv = 1;
        gui_set_event(fi);
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
                if (finder_event_is_set(fi->work_term_event))
                {
                    break;
                }
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
    finder_list_clear(fi->main_to_work_list, 1024, 1024);
    finder_list_clear(fi->work_to_main_list, 1024, 1024);
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
        gui_add_one(fi, wi->filename, wi->in_subfolder, "2", "3");
        free(wi->filename);
        free(wi->in_subfolder);
        free(wi->size);
        free(wi->modified);
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
    int cont;
    int count;
    struct work_item* wi;

    //writeln(fi, "event_callback:");
    count = 0;
    cont = 1;
    while (cont)
    {
        finder_mutex_lock(fi->list_mutex);
        list_count = finder_list_get_count(fi->work_to_main_list);
        if (list_count > 0)
        {
            wi = (struct work_item*)finder_list_get_item(fi->work_to_main_list, 0);
            finder_list_remove_item(fi->work_to_main_list, 0);
            finder_mutex_unlock(fi->list_mutex);
            main_process_work_item(fi, wi);
            count++;
            if (count > 1024)
            {
                //writeln(fi, "event_callback: later");
                /* do more later */
                gui_set_event(fi);
                cont = 0;
            }
        }
        else
        {
            finder_mutex_unlock(fi->list_mutex);
            cont = 0;
        }
    }
    return 0;
}

