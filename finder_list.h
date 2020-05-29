/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2018-2020
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

#if !defined(FINDER_LIST_H)
#define FINDER_LIST_H

#define ITYPE size_t

#ifdef __cplusplus
extern "C" {
#endif

int
finder_list_create(int grow_by, int alloc_size, void** list1);
void
finder_list_delete(void* list1);
int
finder_list_add_item(void* list1, ITYPE item);
ITYPE
finder_list_get_item(const void* list1, int index);
int
finder_list_clear(void* list1, int grow_by, int alloc_size);
int
finder_list_index_of(const void* list1, ITYPE item);
void
finder_list_remove_item(void* list1, int index);
int
finder_list_insert_item(void* list1, int index, ITYPE item);
int
finder_list_get_count(const void* list1);

#ifdef __cplusplus
}
#endif

#endif

