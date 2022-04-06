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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include "finder.h"
#include "finder_event.h"
#include "finder_gui.h"
#include "finder_gui_about.h"
#include "finder_mainwindow.h"

/*****************************************************************************/
static int
gui_create(int argc, char** argv, struct finder_info** fi)
{
    struct gui_object* go;

    *fi = (struct finder_info*)calloc(1, sizeof(struct finder_info));
    mw_create(*fi, &go, argc, argv);
    (*fi)->gui_obj = go;
    return 0;
}

/*****************************************************************************/
static int
gui_main_loop(struct finder_info* fi)
{
    struct gui_object* go;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    mw_main_loop(go);
    return 0;
}

/*****************************************************************************/
static int
gui_delete(struct finder_info* fi)
{
    struct gui_object* go;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    mw_delete(go);
    free(fi);
    return 0;
}

/*****************************************************************************/
int
main(int argc, char** argv)
{
    struct finder_info* fi;

    gui_create(argc, argv, &fi);
    LOGLN0((fi, LOG_INFO, LOGS "gtk run time version %d.%d.%d "
            "compile time version %d.%d.%d", LOGP,
            gtk_major_version, gtk_minor_version, gtk_micro_version,
            GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION));
    gui_main_loop(fi);
    gui_delete(fi);
    return 0;
}

/*****************************************************************************/
/* any thread */
int
gui_set_event(struct finder_info* fi)
{
    struct gui_object* go;

    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    return mw_set_event(go);
}

/*****************************************************************************/
int
gui_find_done(struct finder_info* fi)
{
    struct gui_object* go;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    return mw_find_done(go);
}

/*****************************************************************************/
int
gui_add_one(struct finder_info* fi, const char* filename,
            const char* in_subfolder, FINDER_I64 size,
            const char* modified)
{
    struct gui_object* go;

    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    return mw_add_one(go, filename, in_subfolder, size, modified);
}

/*****************************************************************************/
int
gui_writeln(struct finder_info* fi, const char* msg)
{
    (void)fi;
    printf("%s\n", msg);
    return 0;
}

