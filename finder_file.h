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

#if !defined(FINDER_FILE_H)
#define FINDER_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

int
finder_file_read_sections(const char* file_name, void* names_list);
int
finder_file_read_section(const char* file_name, const char* section,
                         void* names_list, void* values_list);
int
finder_file_write_section(const char* file_name, const char* section,
                          void* names_list, void* values_list);

#ifdef __cplusplus
}
#endif

#endif

