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
    GtkWidget* tab1_fixed;
    GtkWidget* tab1_label;
    struct _name_tab
    {
        GtkWidget* label1;
        GtkWidget* combo1;
        GtkWidget* label2;
        GtkWidget* combo2;
        GtkWidget* but1; /* Browse */
        GtkWidget* cb1;
        GtkWidget* cb2;
        GtkWidget* cb3;
    } name_tab;
    GtkWidget* tab2_fixed;
    GtkWidget* tab2_label;
    struct _date_tab
    {
        GtkWidget* rb1;
        GtkWidget* rb2;
        GtkWidget* combo1;
        GtkWidget* rb3;
        GtkWidget* rb4;
        GtkWidget* rb5;
        GtkWidget* spinner1;
        GtkWidget* spinner2;
        GtkWidget* label1;
        GtkWidget* label2;
    } date_tab;
    GtkWidget* tab3_fixed;
    GtkWidget* tab3_label;
    struct _adva_tab
    {
        GtkWidget* cb1; /* search in files */
        GtkWidget* combo1;
        GtkWidget* cb2; /* case sensitive search */
    } adva_tab;
    GtkWidget* tv1;
    GtkWidget* tv1_scroll;

    GtkWidget* sb;
    GtkWidget* sb1;

    GtkWidget* mw_fixed; /* fixed for mw */

    GtkWidget* menubar;
    GtkWidget* file_menu;
    GtkWidget* help_menu;
    GtkWidget* file_mi;
    GtkWidget* quit_mi;
    GtkWidget* help_mi;
    GtkWidget* helpDDD_mi;
    GtkWidget* about_mi;

    int width;
    int height;

    void* gui_event;
    void* mutex1;

};

int
mw_create(struct finder_info* fi, struct gui_object** ago,
          int argc, char** argv);
int
mw_main_loop(struct gui_object* go);
int
mw_delete(struct gui_object* go);
int
mw_set_event(struct gui_object* go);
int
mw_find_done(struct gui_object* go);
int
mw_add_one(struct gui_object* go, const char* filename,
           const char* in_subfolder, FINDER_I64 size,
           const char* modified);

#endif
