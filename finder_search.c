
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "finder.h"
#include "finder_gui.h"
#include "finder_list.h"
#include "finder_thread.h"
#include "finder_event.h"
#include "finder_search.h"

/*****************************************************************************/
static int
format_commas(off_t n, char* out)
{
    int c;
    char buf[64];
    char* p;

    snprintf(buf, 64, "%lld", (long long)n);
    c = 2 - (strlen(buf) % 3);
    for (p = buf; *p != 0; p++)
    {
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
        if ((chr1 >= 'a') && (chr1 <= 'z'))
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

    data = (char*)malloc(4096);
    found_in_file = 0;
    text_bytes = strlen(fi->text);
    readed = read(fd, data, 4096);
    while (readed > 0)
    {
        count = readed - text_bytes;
        for (index = 0; index < count; index++)
        {
            if (fi->search_in_case_sensitive)
            {
                if (lmemcmp(fi->text, data + index, text_bytes) == 0)
                {
                    found_in_file = 1;
                    break;
                }
            }
            else
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
        if (readed == 4096)
        {
            /* only rewind a bit when we got a full read last time */
            lseek(fd, SEEK_CUR, -text_bytes);
        }
        readed = read(fd, data, 4096);
    }
    free(data);
    *afound_in_file = found_in_file;
    return 0;
}

/*****************************************************************************/
void
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
    char* path;
    char* look_in_text;
    struct stat lstat;

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

        if (finder_event_is_set(fi->work_term_event))
        {
            break;
        }

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
                if ((entry->d_name[0] == '.') && (fi->show_hidden == 0))
                {
                    continue;
                }
                path = (char*)malloc(4096);
                snprintf(path, 4096, "%s/%s", name, entry->d_name);
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
            path = (char*)malloc(4096);
            snprintf(path, 4096, "%s/%s", name, entry->d_name);
            if (fi->search_in_files)
            {
                fd = open(path, O_RDONLY);
                if (fd < 0)
                {
                    writeln(fi, "open error %s", path);
                    free(path);
                    continue;
                }
                if (fstat(fd, &lstat) != 0) 
                {
                    writeln(fi, "fstat error %s", path);
                    free(path);
                    close(fd);
                    continue;
                }
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
                close(fd);
            }
            else
            {
                if (stat(path, &lstat) != 0)
                {
                    writeln(fi, "stat error %s", path);
                    free(path);
                    continue;
                }
            }
            free(path);
            lwi = (struct work_item *)calloc(1, sizeof(struct work_item));
            if (lwi != NULL)
            {
                lwi->cmd = FINDER_CMD_ADD_ONE;
                lwi->filename = strdup(entry->d_name);
                lwi->in_subfolder = strdup(look_in_text);
                lwi->size = (char*)malloc(1024);
                format_commas(lstat.st_size, lwi->size);
                finder_mutex_lock(fi->list_mutex);
                finder_list_add_item(fi->work_to_main_list, (ITYPE)lwi);
                finder_mutex_unlock(fi->list_mutex);
            }
            count++;
            gui_set_event(fi);
        }
    }
    free(look_in_text);
    closedir(dir);
    gui_set_event(fi);
}

