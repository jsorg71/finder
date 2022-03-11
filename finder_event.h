/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2018-2022
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

#if !defined(FINDER_EVENT_H)
#define FINDER_EVENT_H

#if defined(_WIN32)
#define FINDER_WAIT_OBJ HANDLE
#else
#define FINDER_WAIT_OBJ int
#endif

#ifdef __cplusplus
extern "C" {
#endif

int
finder_event_create(void** event1);
int
finder_event_delete(void* event1);
int
finder_event_set(void* event1);
int
finder_event_clear(void* event1);
int
finder_event_is_set(void* event1);
FINDER_WAIT_OBJ
finder_event_get_wait_obj(void* event1);
int
finder_wait(int num_wait_objs, FINDER_WAIT_OBJ* wait_objs);

#ifdef __cplusplus
}
#endif

#endif

