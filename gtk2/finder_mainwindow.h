/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2022
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

#if !defined(FINDER_MAINWINDOW_H)
#define FINDER_MAINWINDOW_H

struct gui_object
{
    struct finder_info* fi;
    GtkWidget* mw; /* main window */
    GtkWidget* but1; /* Find */
    GtkWidget* but2; /* Stop */
    GtkWidget* but3; /* Exit */
    GtkWidget* notebook;
    GtkWidget* fixed;
    int width;
    int height;
};

int
gui_object_create(struct finder_info* fi, struct gui_object** ago,
                  int argc, char** argv);
int
gui_object_mainloop(struct gui_object* go);
int
gui_object_delete(struct gui_object* go);

#endif
