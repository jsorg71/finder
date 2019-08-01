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
#define FINDER_VERSION_MINOR    2

/* main to work */
#define FINDER_CMD_START        1
/* work to main */
#define FINDER_CMD_DONE         101
#define FINDER_CMD_ADD_ONE      102

#if defined(_WIN32)
#define FINDER_I64 INT64
#define FINDER_INTPTR INT_PTR
#else
#define FINDER_I64 long long
#define FINDER_INTPTR long
#endif

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
writeln(struct finder_info* fi, const char* format, ...);
int
start_find(struct finder_info* fi);
int
stop_find(struct finder_info* fi);
int
event_callback(struct finder_info* fi);
int
format_commas(FINDER_I64 n, char* out);
int
get_mstime(void);

#ifdef __cplusplus
}
#endif

#endif

