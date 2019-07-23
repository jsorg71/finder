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
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "finder_file.h"

#define FINDER_FILE_MAX_LINE_BYTES 2048

/*****************************************************************************/
int
finder_file_read_sections(const char* file_name, void* names_list)
{
    (void)file_name;
    (void)names_list;
    return 0;
}

/*****************************************************************************/
int
finder_file_read_section(const char* file_name, const char* section,
                         void* names_list, void* values_list)
{
    (void)file_name;
    (void)section;
    (void)names_list;
    (void)values_list;
    return 0;
}

/*****************************************************************************/
int
finder_file_write_section(const char* file_name, const char* section,
                          void* names_list, void* values_list)
{
    (void)file_name;
    (void)section;
    (void)names_list;
    (void)values_list;
    return 0;
}

