
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "finder.h"
#include "finder_gui.h"
#include "finder_list.h"
#include "finder_thread.h"
#include "finder_event.h"
#include "finder_search.h"

#define SEARCH_IN_READ_CHUCK 4096
#define FINDER_MAX_PATH 4096

/*****************************************************************************/
/* return 0 found, 1 not found */
static int
myfnmatch(const char* pattern, const char* string, int flags)
{
    char c;
    char chr1;
    char chr2;

    while (1)
    {
        switch (c = *(pattern++))
        {
            case 0:
                return (*string) == 0 ? 0 : 1;
            case '?':
                if ((*string) == 0)
                {
                    return 1; /* not found */
                }
                ++string;
                break;
            case '*':
                c = *pattern;
                /* multiple stars */
                while (c == '*')
                {
                    c = *(++pattern);
                }
                if (c == 0)
                {
                    return 0; /* found */
                }
                /* General case, use recursion. */
                while ((*string) != 0)
                {
                    if (!myfnmatch(pattern, string, flags))
                    {
                        return 0; /* found */
                    }
                    ++string;
                }
                return 1; /* not found */
            default:
                if (flags & 1)
                {
                    /* not case sensitive */
                    chr1 = c;
                    if ((chr1 >= 'a') && (chr1 <= 'z'))
                    {
                        chr1 &= 0xDF;
                    }
                    chr2 = *(string++);
                    if ((chr2 >= 'a') && (chr2 <= 'z'))
                    {
                        chr2 &= 0xDF;
                    }
                    if (chr1 != chr2)
                    {
                        return 1; /* not found */
                    }
                }
                else
                {
                    /* case sensitive */
                    if (c != *(string++))
                    {
                        return 1; /* not found */
                    }
                }
                break;
        }
    }
    return 1; /* not found */
}

/*****************************************************************************/
static int
lcasememcmp(const void* ptr1, const void* ptr2, int bytes)
{
    int index;
    const char* ptr18;
    const char* ptr28;
    char chr1;
    char chr2;

    ptr18 = (const char*)ptr1;
    ptr28 = (const char*)ptr2;
    for (index = 0; index < bytes; index++)
    {
        chr1 = ptr18[index];
        if ((chr1 >= 'a') && (chr1 <= 'z'))
        {
            chr1 &= 0xDF;
        }
        chr2 = ptr28[index];
        if ((chr2 >= 'a') && (chr2 <= 'z'))
        {
            chr2 &= 0xDF;
        }
        if (chr1 < chr2)
        {
            return -1;
        }
        if (chr1 > chr2)
        {
            return 1;
        }
    }
    return 0;
}

/*****************************************************************************/
static int
lmemcmp(const void* ptr1, const void* ptr2, int bytes)
{
    int index;
    const char* ptr18;
    const char* ptr28;
    char chr1;
    char chr2;

    ptr18 = (const char*)ptr1;
    ptr28 = (const char*)ptr2;
    for (index = 0; index < bytes; index++)
    {
        chr1 = ptr18[index];
        chr2 = ptr28[index];
        if (chr1 < chr2)
        {
            return -1;
        }
        if (chr1 > chr2)
        {
            return 1;
        }
    }
    return 0;
}

/*****************************************************************************/
static int
find_in_file(int fd, struct finder_info* fi, int* afound_in_file)
{
    int found_in_file;
    int text_bytes;
    int readed;
    int index;
    int count;
    char* data;

    data = (char*)malloc(SEARCH_IN_READ_CHUCK);
    if (data == NULL)
    {
        return 1;
    }
    found_in_file = 0;
    text_bytes = strlen(fi->text);
    readed = read(fd, data, SEARCH_IN_READ_CHUCK);
    while (readed > 0)
    {
        count = readed - text_bytes;
        if (fi->search_in_case_sensitive)
        {
            for (index = 0; index < count; index++)
            {
                if (lmemcmp(fi->text, data + index, text_bytes) == 0)
                {
                    found_in_file = 1;
                    break;
                }
            }
        }
        else
        {
            for (index = 0; index < count; index++)
            {
                if (lcasememcmp(fi->text, data + index, text_bytes) == 0)
                {
                    found_in_file = 1;
                    break;
                }
            }
        }
        if (found_in_file)
        {
            break;
        }
        if (finder_event_is_set(fi->work_term_event))
        {
            break;
        }
        if (readed == SEARCH_IN_READ_CHUCK)
        {
            /* only rewind a bit when we got a full read last time */
            lseek(fd, SEEK_CUR, -text_bytes);
        }
        readed = read(fd, data, SEARCH_IN_READ_CHUCK);
    }
    free(data);
    *afound_in_file = found_in_file;
    return 0;
}

/*****************************************************************************/
/* return 0 if match */
static int
check_file_name(struct finder_info* fi, const char* filename)
{

    const char* p1;
    const char* p2;
    char* text;
    int len1;

    text = (char*)malloc(1024);
    p1 = fi->named;
    while (p1 != NULL)
    {
        p2 = strstr(p1 + 1, ";");
        if (p2 == NULL)
        {
            strncpy(text, p1, 1023);
            text[1023] = 0;
        }
        else
        {
            len1 = p2 - p1;
            //writeln(fi, "len1 %d", len1);
            if (len1 > 1023)
            {
                len1 = 1023;
            }
            strncpy(text, p1, len1);
            text[len1] = 0;
            p2++;
        }
        if (text[0] != 0)
        {
            //writeln(fi, "%s", text);
            if (fi->case_sensitive)
            {
                if (myfnmatch(text, filename, 0) == 0)
                {
                    free(text);
                    return 0;
                }
            }
            else
            {
                if (myfnmatch(text, filename, 1) == 0)
                {
                    free(text);
                    return 0;
                }
            }
        }
        p1 = p2;
    }
    free(text);
    return 1;
}

/*****************************************************************************/
int
listdir(struct finder_info* fi, struct work_item* wi, const char* name)
{
    DIR* dir;
    struct dirent* entry;
    struct work_item* lwi;
    int count;
    int look_in_bytes;
    int name_bytes;
    int look_in_text_alloc_bytes;
    int fd;
    int found_in_file;
    int is_dir;
    int got_stat;
    char* path;
    char* look_in_text;
    struct stat lstat1;
    struct tm* local_time;

    look_in_bytes = strlen(fi->look_in);
    name_bytes = strlen(name);
    //writeln(fi, "%d %d", look_in_bytes, name_bytes);
    if (name_bytes < look_in_bytes)
    {
        writeln(fi, "%s", "error");
        return 1;
    }

    dir = opendir(name);
    if (dir == NULL)
    {
        writeln(fi, "%s", "error");
        return 1;
    }

    look_in_text_alloc_bytes = name_bytes - look_in_bytes + 1;
    look_in_text = (char*)calloc(look_in_text_alloc_bytes, 1);
    if (look_in_text == NULL)
    {
        writeln(fi, "%s", "error");
        return 1;
    }

    if (name_bytes > look_in_bytes)
    {
        snprintf(look_in_text, look_in_text_alloc_bytes, "%s", name + look_in_bytes + 1);
    }
    count = 0;
    while (1)
    {
        if (finder_event_is_set(fi->work_term_event))
        {
            break;
        }
        entry = readdir(dir);
        if (entry == NULL)
        {
            break;
        }
        fd = -1;
        got_stat = 0;
        path = (char*)malloc(FINDER_MAX_PATH);
        if (path == NULL)
        {
            continue;
        }
        snprintf(path, FINDER_MAX_PATH, "%s/%s", name, entry->d_name);
        if ((entry->d_type == DT_UNKNOWN) || fi->search_in_files)
        {
            /* much slower if we have to go in here */
            fd = open(path, O_RDONLY);
            if (fd < 0)
            {
                writeln(fi, "open error %s", path);
                free(path);
                close(fd);
                continue;
            }
            if (fstat(fd, &lstat1) != 0)
            {
                writeln(fi, "fstat error %s", path);
                free(path);
                close(fd);
                continue;
            }
            got_stat = 1;
            is_dir = (lstat1.st_mode & S_IFMT) == S_IFDIR;
        }
        else
        {
            is_dir = entry->d_type == DT_DIR;
        }
        if (is_dir)
        {
            if ((strcmp(entry->d_name, ".") == 0) ||
                (strcmp(entry->d_name, "..") == 0))
            {
                free(path);
                close(fd);
                continue;
            }
            if (fi->include_subfolders)
            {
                if ((entry->d_name[0] == '.') && (fi->show_hidden == 0))
                {
                    free(path);
                    close(fd);
                    continue;
                }
                listdir(fi, wi, path);
            }
        }
        else
        {
            if ((entry->d_name[0] == '.') && (fi->show_hidden == 0))
            {
                free(path);
                close(fd);
                continue;
            }
            if (check_file_name(fi, entry->d_name) != 0)
            {
                free(path);
                close(fd);
                continue;
            }
            if (fi->search_in_files)
            {
                if (find_in_file(fd, fi, &found_in_file) != 0)
                {
                    free(path);
                    close(fd);
                    continue;
                }
                if (!found_in_file)
                {
                    free(path);
                    close(fd);
                    continue;
                }
            }
            lwi = (struct work_item*)calloc(1, sizeof(struct work_item));
            if (lwi != NULL)
            {
                if (got_stat == 0)
                {
                    //writeln(fi, "stat %s", path);
                    if (stat(path, &lstat1) != 0)
                    {
                        writeln(fi, "stat error %s", path);
                        free(path);
                        close(fd);
                        continue;
                    }
                }
                lwi->cmd = FINDER_CMD_ADD_ONE;
                lwi->filename = strdup(entry->d_name);
                lwi->in_subfolder = strdup(look_in_text);
                lwi->size = lstat1.st_size;
                local_time = localtime(&(lstat1.st_mtim.tv_sec));
                if (local_time != NULL)
                {
                    lwi->modified = (char*)malloc(1024);
                    if (lwi->modified != NULL)
                    {
                        snprintf(lwi->modified, 1024, "%4.4d%2.2d%2.2d %2.2d:%2.2d:%2.2d",
                                 1900 + local_time->tm_year, local_time->tm_mon, local_time->tm_mday,
                                 local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
                    }
                }
                finder_mutex_lock(fi->list_mutex);
                finder_list_add_item(fi->work_to_main_list, (ITYPE)lwi);
                finder_mutex_unlock(fi->list_mutex);
            }
            count++;
            gui_set_event(fi);
        }
        free(path);
        close(fd);
    }
    free(look_in_text);
    closedir(dir);
    gui_set_event(fi);
    return 0;
}

