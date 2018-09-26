/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2018
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

#if !defined(FINDER_GUI_H)
#define FINDER_GUI_H

#ifdef __cplusplus
extern "C" {
#endif

int
gui_set_event(struct finder_info* fi);
int
gui_find_done(struct finder_info* fi);
int
gui_add_one(struct finder_info* fi, const char* filename,
            const char* in_subfolder, FINDER_I64 size,
            const char* modified);

#ifdef __cplusplus
}
#endif

#endif

