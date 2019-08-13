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

#if !defined(FINDER_H)
#define FINDER_H

#define FINDER_VERSION_MAJOR    0
#define FINDER_VERSION_MINOR    4

/* main to work */
#define FINDER_CMD_START        1
/* work to main */
#define FINDER_CMD_DONE         101
#define FINDER_CMD_ADD_ONE      102

#if defined(_WIN32)
#define FINDER_I64 __int64
#define FINDER_PRId64 "Ld"
#else
#define FINDER_I64 long long
#define FINDER_PRId64 "lld"
#endif

#if defined(_MSC_VER)
#define FINDER_SNPRINTF _snprintf
#define FINDER_VSNPRINTF _vsnprintf
#else
#define FINDER_SNPRINTF snprintf
#define FINDER_VSNPRINTF vsnprintf
#endif

#define FINDER_STRLEN(_str) ((int)strlen(_str))

/* strdup that can handle NULL */
#define SAFESTRDUP(_str) ((_str) == NULL) ? NULL : strdup(_str)

struct work_item
{
    int cmd;
    int pad0;
    char* filename;
    char* in_subfolder;
    FINDER_I64 size;
    char* modified;
};

struct finder_info
{
    void* gui_obj;
    char named[256];
    char look_in[256];
    int include_subfolders;
    int case_sensitive;
    int show_hidden;
    int search_in_files;
    int search_in_case_sensitive;
    int pad0;
    char text[256];

    void* work_term_event;
    void* main_to_work_event;
    void* list_mutex;
    void* main_to_work_list;
    void* work_to_main_list;
};

#ifdef __cplusplus
extern "C" {
#endif

int
gui_init(struct finder_info* fi);
int
gui_deinit(struct finder_info* fi);
int
start_find(struct finder_info* fi);
int
stop_find(struct finder_info* fi);
int
event_callback(struct finder_info* fi);
int
format_commas(FINDER_I64 n, char* out, int out_bytes);
int
get_mstime(void);

int
logln(struct finder_info* fi, int log_level, const char* format, ...);

#define LOG_ERROR 0
#define LOG_WARN  1
#define LOG_INFO  2
#define LOG_DEBUG 3

#define LOGS "%s(%d):%s:"
#define LOGP __FILE__, __LINE__, __FUNCTION__

#if !defined(__FUNCTION__) && defined(__FUNC__)
#define LOG_PRE const char* __FUNCTION__ = __FUNC__; (void)__FUNCTION__;
#else
#define LOG_PRE
#endif

#define LOG_LEVEL 1
#if LOG_LEVEL > 0
#define LOGLN0(_args) do { LOG_PRE logln _args ; } while (0)
#else
#define LOGLN0(_args)
#endif
#if LOG_LEVEL > 10
#define LOGLN10(_args) do { LOG_PRE logln _args ; } while (0)
#else
#define LOGLN10(_args)
#endif

#ifdef __cplusplus
}
#endif

#endif

