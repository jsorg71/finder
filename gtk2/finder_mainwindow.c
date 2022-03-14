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
static gboolean
configure_callback(GtkWindow* window, GdkEvent* event, gpointer data)
{
    struct gui_object* go;
    struct finder_info* fi;
    GdkEventConfigure* levent;

    levent = (GdkEventConfigure*)event;
    fi = (struct finder_info*)data;
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    if (GTK_WIDGET(window) != go->mw)
    {
        return FALSE;
    }
    if ((go->width != levent->width) || (go->height != levent->height))
    {
        LOGLN0((fi, LOG_DEBUG, LOGS "resized to %dx%d, was %dx%d",
                LOGP, levent->width, levent->height, go->width, go->height));
        go->width = levent->width;
        go->height = levent->height;

        gtk_widget_set_uposition(GTK_WIDGET(go->but1), go->width - 110, 32);
        gtk_widget_set_size_request(GTK_WIDGET(go->but1), 100, 30);

        gtk_widget_set_uposition(GTK_WIDGET(go->but2), go->width - 110, 72);
        gtk_widget_set_size_request(GTK_WIDGET(go->but2), 100, 30);

        gtk_widget_set_uposition(GTK_WIDGET(go->but3), go->width - 110, 165);
        gtk_widget_set_size_request(GTK_WIDGET(go->but3), 100, 30);

        gtk_widget_set_uposition(GTK_WIDGET(go->notebook), 8, 28);
        gtk_widget_set_size_request(GTK_WIDGET(go->notebook), go->width - 126, 168);

    }
    return FALSE;
}

/*****************************************************************************/
int
gui_object_create(struct finder_info* fi, struct gui_object** ago,
                  int argc, char** argv)
{
    struct gui_object* go;

    go = (struct gui_object*)calloc(1, sizeof(struct gui_object));
    go->fi = fi;
    gtk_init(&argc, &argv);
    go->mw = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(go->mw), "Finder");
    gtk_window_set_default_size(GTK_WINDOW(go->mw), 640, 480);
    gtk_widget_add_events(GTK_WIDGET(go->mw), GDK_CONFIGURE);

    go->fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(go->mw), GTK_WIDGET(go->fixed));

    go->but1 = gtk_button_new_with_label("Find");
    gtk_container_add(GTK_CONTAINER(go->fixed), GTK_WIDGET(go->but1));

    go->but2 = gtk_button_new_with_label("Stop");
    gtk_container_add(GTK_CONTAINER(go->fixed), GTK_WIDGET(go->but2));

    go->but3 = gtk_button_new_with_label("Exit");
    gtk_container_add(GTK_CONTAINER(go->fixed), GTK_WIDGET(go->but3));

    go->notebook = gtk_notebook_new();
    gtk_notebook_append_page(GTK_NOTEBOOK(go->notebook), gtk_fixed_new(), 0);
    gtk_container_add(GTK_CONTAINER(go->fixed), GTK_WIDGET(go->notebook));

    g_signal_connect(G_OBJECT(go->mw), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(go->mw), "configure-event",
                     G_CALLBACK(configure_callback), fi);

    gtk_widget_show_all(GTK_WIDGET(go->mw));

    *ago = go;
    return 0;
}

/*****************************************************************************/
int
gui_object_mainloop(struct gui_object* go)
{
    gui_init(go->fi);
    gtk_main();
    gui_deinit(go->fi);
    return 0;
}

/*****************************************************************************/
int
gui_object_delete(struct gui_object* go)
{
    free(go);
    return 0;
}

