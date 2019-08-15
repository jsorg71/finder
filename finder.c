
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include "finder.h"
#include "finder_gui.h"
#include "finder_list.h"
#include "finder_thread.h"
#include "finder_event.h"
#include "finder_search.h"

/*****************************************************************************/
/* main thread */
int
gui_init(struct finder_info* fi)
{
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
#include "finder_warn_off.h"
    if (sizeof(FINDER_I64) != 8)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "bad FINDER_I64 size", LOGP));
        return 1;
    }
    if (sizeof(ITYPE) != sizeof(void*))
    {
        LOGLN0((fi, LOG_ERROR, LOGS "bad ITYPE size", LOGP));
        return 1;
    }
#include "finder_warn_on.h"
    if (finder_mutex_create(&(fi->list_mutex)) != 0)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "finder_mutex_create failed", LOGP));
        return 1;
    }
    if (finder_event_create(&(fi->work_term_event)) != 0)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "finder_event_create failed", LOGP));
        finder_mutex_delete(fi->list_mutex);
        return 2;
    }
    if (finder_event_create(&(fi->main_to_work_event)) != 0)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "finder_event_create failed", LOGP));
        finder_mutex_delete(fi->list_mutex);
        finder_event_delete(fi->work_term_event);
        return 2;
    }
    if (finder_list_create(1024, 1024, &(fi->main_to_work_list)) != 0)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "finder_list_create failed", LOGP));
        finder_mutex_delete(fi->list_mutex);
        finder_event_delete(fi->work_term_event);
        finder_event_delete(fi->main_to_work_event);
        return 3;
    }
    if (finder_list_create(1024, 1024, &(fi->work_to_main_list)) != 0)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "finder_list_create failed", LOGP));
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
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    finder_mutex_delete(fi->list_mutex);
    finder_event_delete(fi->work_term_event);
    finder_event_delete(fi->main_to_work_event);
    finder_list_delete(fi->main_to_work_list);
    finder_list_delete(fi->work_to_main_list);
    return 0;
}

/*****************************************************************************/
/* work thread */
static int
process_work_item(struct finder_info* fi, struct work_item* wi)
{
    int rv;

    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    if (wi == NULL)
    {
        return 0;
    }
    rv = 0;
    LOGLN10((fi, LOG_INFO, LOGS "cmd %d", LOGP, wi->cmd));
    if (wi->cmd == FINDER_CMD_START)
    {
        LOGLN10((fi, LOG_INFO, LOGS "fi %p wi %p look_in %p", LOGP, fi, wi, fi->look_in));
        listdir(fi, wi, fi->look_in);
        rv = 1;
        gui_set_event(fi);
    }
    LOGLN10((fi, LOG_INFO, LOGS "free item", LOGP));
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
    FINDER_WAIT_OBJ wos[2];
    struct finder_info* fi;
    struct work_item* wi;

    fi = (struct finder_info*)arg;
    LOGLN0((fi, LOG_INFO, LOGS "thread start", LOGP));
    cont = 1;
    while (cont)
    {
        LOGLN10((fi, LOG_INFO, LOGS "loop", LOGP));
        wos[0] = finder_event_get_wait_obj(fi->work_term_event);
        wos[1] = finder_event_get_wait_obj(fi->main_to_work_event);
        error = finder_wait(2, wos);
        if (error < 0)
        {
            LOGLN0((fi, LOG_ERROR, LOGS "finder_wait failed", LOGP));
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
    LOGLN0((fi, LOG_INFO, LOGS "thread stop", LOGP));
    return 0;
}

/*****************************************************************************/
/* main thread */
int
start_find(struct finder_info* fi)
{
    struct work_item* wi;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    LOGLN0((fi, LOG_INFO, "  named [%s]", fi->named));
    LOGLN0((fi, LOG_INFO, "  look_in [%s]", fi->look_in));
    LOGLN0((fi, LOG_INFO, "  include_subfolders %d", fi->include_subfolders));
    LOGLN0((fi, LOG_INFO, "  case_sensitive %d", fi->case_sensitive));
    LOGLN0((fi, LOG_INFO, "  show_hidden %d", fi->show_hidden));
    LOGLN0((fi, LOG_INFO, "  search_in_files %d", fi->search_in_files));
    LOGLN0((fi, LOG_INFO, "  case search_in_case_sensitive %d", fi->search_in_case_sensitive));
    LOGLN0((fi, LOG_INFO, "  text [%s]", fi->text));

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
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    finder_event_set(fi->work_term_event);
    finder_mutex_lock(fi->list_mutex);
    finder_list_clear(fi->main_to_work_list, 1024, 1024);
    finder_list_clear(fi->work_to_main_list, 1024, 1024);
    finder_mutex_unlock(fi->list_mutex);
    gui_find_done(fi);
    return 0;
}

/*****************************************************************************/
/* main thread */
static int
main_process_work_item(struct finder_info* fi, struct work_item* wi)
{
    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    if (wi == NULL)
    {
        return 0;
    }
    LOGLN10((fi, LOG_INFO, LOGS "cmd %d", LOGP, wi->cmd));
    if (wi->cmd == FINDER_CMD_DONE)
    {
        gui_find_done(fi);
    }
    else if (wi->cmd == FINDER_CMD_ADD_ONE)
    {
        gui_add_one(fi, wi->filename, wi->in_subfolder, wi->size,
                    wi->modified);
        free(wi->filename);
        free(wi->in_subfolder);
        free(wi->modified);
    }
    LOGLN10((fi, LOG_INFO, LOGS "free item", LOGP));
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

    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    count = 0;
    cont = 1;
    while (cont)
    {
        finder_mutex_lock(fi->list_mutex);
        list_count = finder_list_get_count(fi->work_to_main_list);
        if (list_count > 0)
        {
            wi = (struct work_item*)
                 finder_list_get_item(fi->work_to_main_list, 0);
            finder_list_remove_item(fi->work_to_main_list, 0);
            finder_mutex_unlock(fi->list_mutex);
            main_process_work_item(fi, wi);
            count++;
            if (count > 1024)
            {
                LOGLN10((fi, LOG_INFO, LOGS "later", LOGP));
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

/*****************************************************************************/
int
format_commas(FINDER_I64 n, char* out, int out_bytes)
{
    int c;
    char buf[64];
    char* p;
    char* out_end;

    if (out == NULL)
    {
        return 1;
    }
    out_end = out + out_bytes - 2;
    finder_snprintf(buf, 64, "%" FINDER_PRId64, n);
    c = 2 - (FINDER_STRLEN(buf) % 3);
    for (p = buf; *p != 0; p++)
    {
        if (out >= out_end)
        {
            return 1;
        }
        *(out++) = *p;
        if (c == 1)
        {
           *(out++) = ',';
        }
        c = (c + 1) % 3;
    }
    *(--out) = 0;
    return 0;
}

/*****************************************************************************/
int
get_mstime(void)
{
#if defined(_WIN32)
    return GetTickCount();
#else
    struct timeval tp;

    gettimeofday(&tp, 0);
    return (tp.tv_sec * 1000) + (tp.tv_usec / 1000);
#endif
}

static int g_log_level = 4;

static const char g_log_pre[][8] =
{
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG"
};

/*****************************************************************************/
int
finder_snprintf(char* buffer, size_t count, const char *format, ...)
{
    va_list ap;
    int rv;

    va_start(ap, format);
#if defined(_MSC_VER)
    rv = _vsnprintf_s(buffer, count, count, format, ap);
#else
    rv = vsnprintf(buffer, count, format, ap);
#endif
    va_end(ap);
    return rv;
}

/*****************************************************************************/
int
finder_vsnprintf(char* buffer, size_t count, const char *format, va_list ap)
{
    int rv;

#if defined(_MSC_VER)
    rv = _vsnprintf_s(buffer, count, count, format, ap);
#else
    rv = vsnprintf(buffer, count, format, ap);
#endif
    return rv;
}

/*****************************************************************************/
char*
finder_strdup(const char* src)
{
    if (src == NULL)
    {
        return NULL;
    }
#if defined(_MSC_VER)
    return _strdup(src);
#else
    return strdup(src);
#endif
}

/*****************************************************************************/
int
finder_stricmp(const char *string1, const char *string2)
{
#if defined(_WIN32)
#if defined(_MSC_VER)
    return _stricmp(string1, string2);
#else
    return stricmp(string1, string2);
#endif
#else
    return strcasecmp(string1, string2);
#endif
}

/*****************************************************************************/
int
logln(struct finder_info* fi, int log_level, const char* format, ...)
{
    va_list ap;
    char* log_line;

    if (fi == NULL)
    {
        return 0;
    }
    if (log_level < g_log_level)
    {
        log_line = (char*)malloc(2048);
        va_start(ap, format);
        finder_vsnprintf(log_line, 1024, format, ap);
        va_end(ap);
        finder_snprintf(log_line + 1024, 1024, "[%10.10u][%s]%s",
                        get_mstime(), g_log_pre[log_level % 4], log_line);
        gui_writeln(fi, log_line + 1024);
        free(log_line);
    }
    return 0;
}
