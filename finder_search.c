/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2018-2019
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "finder.h"
#include "finder_gui.h"
#include "finder_list.h"
#include "finder_thread.h"
#include "finder_event.h"
#include "finder_search.h"

#define SEARCH_IN_READ_CHUCK 4096
#define FINDER_MAX_PATH 4096

#if defined(_WIN32)
#define FINDER_FILE_OBJ HANDLE
#define FINDER_FILE_INVALID INVALID_HANDLE_VALUE
#define FINDER_FILE_OPEN_RO(_path, _file_obj) \
    _file_obj = CreateFileA(_path, GENERIC_READ, \
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, \
                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)
#define FINDER_FILE_CLOSE(_file_obj) \
    if (file_obj != INVALID_HANDLE_VALUE) CloseHandle(_file_obj)
#define FINDER_FILE_READ(_file_obj, _ptr, _bytes_to_read, _bytes_read) \
    do { \
        DWORD lbytes_read; \
        _bytes_read = ReadFile(_file_obj, _ptr, _bytes_to_read, \
                               &lbytes_read, NULL) ? lbytes_read : 0; \
    } while (0)
#define FINDER_FILE_SEEK_CUR(_file_obj, _bytes_move) \
    SetFilePointer(_file_obj, _bytes_move, NULL, FILE_CURRENT)
#else
#define FINDER_FILE_OBJ int
#define FINDER_FILE_INVALID -1
#define FINDER_FILE_OPEN_RO(_path, _file_obj) \
    _file_obj = open(_path, O_RDONLY)
#define FINDER_FILE_CLOSE(_file_obj) if (_file_obj != -1) close(_file_obj)
#define FINDER_FILE_READ(_file_obj, _ptr, _bytes_to_read, _bytes_read) \
    _bytes_read = read(_file_obj, _ptr, _bytes_to_read)
#define FINDER_FILE_SEEK_CUR(_file_obj, _bytes_move) \
    lseek(_file_obj, SEEK_CUR, _bytes_move);
#endif

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
    /* can not get here */
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
find_in_file(FINDER_FILE_OBJ file_obj, struct finder_info* fi,
             int* afound_in_file)
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
    FINDER_FILE_READ(file_obj, data, SEARCH_IN_READ_CHUCK, readed);
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
            writeln(fi, "find_in_file: work_term_event set");
            break;
        }
        if (readed == SEARCH_IN_READ_CHUCK)
        {
            /* only rewind a bit when we got a full read last time */
            FINDER_FILE_SEEK_CUR(file_obj, -text_bytes);
        }
        FINDER_FILE_READ(file_obj, data, SEARCH_IN_READ_CHUCK, readed);
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

#if defined(_WIN32)
#define FINDER_FIND_NEXT_BREAK_CONTINUE \
    if (!FindNextFile(find_handle, &entry)) break; continue
#define FINDER_FIND_CLOSE FindClose(find_handle)
#define FINDER_FIND_FILE_HIDDEN \
    (entry.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
#else
#define FINDER_FIND_NEXT_BREAK_CONTINUE \
    entry = readdir(find_handle); if (!entry) break; continue
#define FINDER_FIND_CLOSE closedir(find_handle)
#define FINDER_FIND_FILE_HIDDEN (entry_file_name[0] == '.')
#endif

/*****************************************************************************/
int
listdir(struct finder_info* fi, struct work_item* wi, const char* dir_name)
{
#if defined(_WIN32)
    HANDLE find_handle;
    WIN32_FIND_DATAA entry;
    SYSTEMTIME system_time;
    SYSTEMTIME local_time;
    char* ldir_name;
#else
    DIR* find_handle;
    struct dirent* entry;
    struct stat lstat1;
    struct tm* local_time;
    int got_stat;
#endif
    struct work_item* lwi;
    int look_in_bytes;
    int dir_name_bytes;
    int in_subfolder_text_alloc_bytes;
    int found_in_file;
    int is_dir;
    int do_open;
    char* dir_file_name;
    char* in_subfolder_text;
    const char* entry_file_name;
    FINDER_FILE_OBJ file_obj;

    look_in_bytes = strlen(fi->look_in);
    dir_name_bytes = strlen(dir_name);
    //writeln(fi, "%d %d", look_in_bytes, dir_name_bytes);
    if (dir_name_bytes < look_in_bytes)
    {
        writeln(fi, "listdir: error, dir_name_bytes can not be less then dir_name_bytes");
        return 1;
    }
#if defined(_WIN32)
    ldir_name = (char*)malloc(FINDER_MAX_PATH);
    if (ldir_name == NULL)
    {
        writeln(fi, "listdir: error malloc");
        return 1;
    }
    snprintf(ldir_name, FINDER_MAX_PATH, "%s\\*", dir_name);
    //writeln(fi, "listdir: ldir_name [%s]", ldir_name);
    find_handle = FindFirstFileA(ldir_name, &entry);
    free(ldir_name);
    if (find_handle == INVALID_HANDLE_VALUE)
    {
        writeln(fi, "listdir: error access directory [%s]", dir_name);
        return 1;
    }
#else
    find_handle = opendir(dir_name);
    if (find_handle == NULL)
    {
        writeln(fi, "listdir: error access directory [%s]", dir_name);
        return 1;
    }
    else
    {
        entry = readdir(find_handle);
        if (entry == NULL)
        {
            writeln(fi, "listdir: error access directory [%s]", dir_name);
            closedir(find_handle);
            return 1;
        }
    }
#endif
    in_subfolder_text_alloc_bytes = dir_name_bytes - look_in_bytes + 1;
    in_subfolder_text = (char*)calloc(in_subfolder_text_alloc_bytes, 1);
    if (in_subfolder_text == NULL)
    {
        writeln(fi, "listdir: error calloc");
        FINDER_FIND_CLOSE;
        return 1;
    }
    dir_file_name = (char*)malloc(FINDER_MAX_PATH);
    if (dir_file_name == NULL)
    {
        writeln(fi, "listdir: error malloc");
        free(in_subfolder_text);
        FINDER_FIND_CLOSE;
        return 1;
    }

    if (dir_name_bytes > look_in_bytes)
    {
        snprintf(in_subfolder_text, in_subfolder_text_alloc_bytes, "%s",
                 dir_name + look_in_bytes + 1);
    }
    while (1)
    {
        if (finder_event_is_set(fi->work_term_event))
        {
            writeln(fi, "listdir: work_term_event set");
            break;
        }
        file_obj = FINDER_FILE_INVALID;
#if defined(_WIN32)
        entry_file_name = entry.cFileName;
        do_open = fi->search_in_files;
        snprintf(dir_file_name, FINDER_MAX_PATH, "%s\\%s", dir_name, entry_file_name);
#else
        entry_file_name = entry->d_name;
        got_stat = 0;
        do_open = (entry->d_type == DT_UNKNOWN) || fi->search_in_files;
        snprintf(dir_file_name, FINDER_MAX_PATH, "%s/%s", dir_name, entry_file_name);
#endif
        if (do_open)
        {
            /* much slower if we have to go in here */
            FINDER_FILE_OPEN_RO(dir_file_name, file_obj);
            if (file_obj == FINDER_FILE_INVALID)
            {
                writeln(fi, "open error %s", dir_file_name);
                FINDER_FILE_CLOSE(file_obj);
                FINDER_FIND_NEXT_BREAK_CONTINUE;
            }
#if defined(_WIN32)
            is_dir = entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
#else
            if (fstat(file_obj, &lstat1) != 0)
            {
                writeln(fi, "fstat error %s", dir_file_name);
                FINDER_FILE_CLOSE(file_obj);
                FINDER_FIND_NEXT_BREAK_CONTINUE;
            }
            got_stat = 1;
            is_dir = (lstat1.st_mode & S_IFMT) == S_IFDIR;
#endif
        }
        else
        {
#if defined(_WIN32)
            is_dir = entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
#else
            is_dir = entry->d_type == DT_DIR;
#endif
        }
        if (is_dir)
        {
            if ((strcmp(entry_file_name, ".") == 0) ||
                (strcmp(entry_file_name, "..") == 0))
            {
                FINDER_FILE_CLOSE(file_obj);
                FINDER_FIND_NEXT_BREAK_CONTINUE;
            }
            if (fi->include_subfolders)
            {
                if (FINDER_FIND_FILE_HIDDEN && (fi->show_hidden == 0))
                {
                    FINDER_FILE_CLOSE(file_obj);
                    FINDER_FIND_NEXT_BREAK_CONTINUE;
                }
                listdir(fi, wi, dir_file_name);
            }
        }
        else
        {
            if (FINDER_FIND_FILE_HIDDEN && (fi->show_hidden == 0))
            {
                FINDER_FILE_CLOSE(file_obj);
                FINDER_FIND_NEXT_BREAK_CONTINUE;
            }
            if (check_file_name(fi, entry_file_name) != 0)
            {
                FINDER_FILE_CLOSE(file_obj);
                FINDER_FIND_NEXT_BREAK_CONTINUE;
            }
            if (fi->search_in_files)
            {
                if (find_in_file(file_obj, fi, &found_in_file) != 0)
                {
                    FINDER_FILE_CLOSE(file_obj);
                    FINDER_FIND_NEXT_BREAK_CONTINUE;
                }
                if (!found_in_file)
                {
                    FINDER_FILE_CLOSE(file_obj);
                    FINDER_FIND_NEXT_BREAK_CONTINUE;
                }
            }
            lwi = (struct work_item*)calloc(1, sizeof(struct work_item));
            if (lwi != NULL)
            {
                lwi->cmd = FINDER_CMD_ADD_ONE;
                lwi->filename = SAFESTRDUP(entry_file_name);
                lwi->in_subfolder = SAFESTRDUP(in_subfolder_text);
#if defined(_WIN32)
                lwi->size = entry.nFileSizeHigh;
                lwi->size = lwi->size << 32;
                lwi->size = lwi->size | entry.nFileSizeLow;
                if (FileTimeToSystemTime(&(entry.ftLastWriteTime),
                                         &system_time))
                {
                    if (SystemTimeToTzSpecificLocalTime(NULL, &system_time,
                                                        &local_time))
                    {
                        lwi->modified = (char*)malloc(1024);
                        if (lwi->modified != NULL)
                        {
                            snprintf(lwi->modified, 1024,
                                     "%4.4d%2.2d%2.2d %2.2d:%2.2d:%2.2d",
                                     local_time.wYear, local_time.wMonth,
                                     local_time.wDay,  local_time.wHour,
                                     local_time.wMinute, local_time.wSecond);
                        }
                        else
                        {
                            writeln(fi, "listdir: malloc error");
                        }
                    }
                    else
                    {
                        writeln(fi, "listdir: SystemTimeToTzSpecificLocalTime failed");
                    }
                }
                else
                {
                    writeln(fi, "listdir: FileTimeToSystemTime failed");
                }
#else
                if (got_stat == 0)
                {
                    //writeln(fi, "listdir: stat %s", dir_file_name);
                    if (stat(dir_file_name, &lstat1) != 0)
                    {
                        writeln(fi, "stat error %s", dir_file_name);
                        FINDER_FILE_CLOSE(file_obj);
                        FINDER_FIND_NEXT_BREAK_CONTINUE;
                    }
                }
                lwi->size = lstat1.st_size;
                local_time = localtime(&(lstat1.st_mtim.tv_sec));
                if (local_time != NULL)
                {
                    lwi->modified = (char*)malloc(1024);
                    if (lwi->modified != NULL)
                    {
                        snprintf(lwi->modified, 1024,
                                 "%4.4d%2.2d%2.2d %2.2d:%2.2d:%2.2d",
                                 1900 + local_time->tm_year,
                                 local_time->tm_mon, local_time->tm_mday,
                                 local_time->tm_hour, local_time->tm_min,
                                 local_time->tm_sec);
                    }
                }
#endif
                //writeln(fi, "listdir: add one filename [%s] modified [%s]", lwi->filename, lwi->modified);
                finder_mutex_lock(fi->list_mutex);
                finder_list_add_item(fi->work_to_main_list, (ITYPE)lwi);
                finder_mutex_unlock(fi->list_mutex);
            }
            else
            {
                writeln(fi, "listdir: malloc error");
            }
            gui_set_event(fi);
        }
        FINDER_FILE_CLOSE(file_obj);
        FINDER_FIND_NEXT_BREAK_CONTINUE;
    }
    free(in_subfolder_text);
    free(dir_file_name);
    FINDER_FIND_CLOSE;
    gui_set_event(fi);
    return 0;
}
