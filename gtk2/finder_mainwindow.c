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

#define GUI_START_WIDTH 640
#define GUI_START_HEIGHT 480

/*****************************************************************************/
static void
lmove_size(GtkWidget* fixed, GtkWidget* widget, int x, int y, int cx, int cy)
{
    gtk_fixed_move(GTK_FIXED(fixed), widget, x, y);
    gtk_widget_set_size_request(widget, cx, cy);
}

/*****************************************************************************/
static void
lmove_size_width_height(struct gui_object* go, int width, int height)
{
    lmove_size(go->fixed, go->but1, width - 110, 32, 100, 30);
    lmove_size(go->fixed, go->but2, width - 110, 72, 100, 30);
    lmove_size(go->fixed, go->but3, width - 110, 165, 100, 30);
    lmove_size(go->fixed, go->notebook, 8, 28, width - 126, 168);
    lmove_size(go->tab1, go->name_tab.label1, 8, 8, 100, 24);
    lmove_size(go->tab1, go->name_tab.combo1, 85, 8, width - 126 - 100, 24);
    lmove_size(go->tab1, go->name_tab.label2, 8, 41, 100, 24);
    lmove_size(go->tab1, go->name_tab.combo2, 85, 41, width - 126 - 165, 24);
    lmove_size(go->tab1, go->name_tab.but1, width - 126 - 75, 41, 60, 24);
    lmove_size(go->tab1, go->name_tab.cb1, 8, 74, 200, 24);
    lmove_size(go->tab1, go->name_tab.cb2, 8, 107, 200, 24);
    lmove_size(go->tab1, go->name_tab.cb3, 216, 74, 200, 24);
    lmove_size(go->tab2, go->date_tab.rb1, 10, 8, 400, 24);
    lmove_size(go->tab2, go->date_tab.rb2, 10, 8 + 24, 400, 24);
    lmove_size(go->tab2, go->date_tab.combo1, 110, 8 + 24, 100, 24);
    lmove_size(go->tab2, go->date_tab.rb3, 30, 8 + 48, 400, 24);
    lmove_size(go->tab2, go->date_tab.rb4, 30, 8 + 72, 400, 24);
    lmove_size(go->tab2, go->date_tab.label1, 250, 8 + 72, 100, 24);
    lmove_size(go->tab2, go->date_tab.rb5, 30, 8 + 96, 400, 24);
    lmove_size(go->tab2, go->date_tab.label2, 250, 8 + 96, 100, 24);
    lmove_size(go->tab2, go->date_tab.spinner1, 200, 8 + 72, 48, 24);
    lmove_size(go->tab2, go->date_tab.spinner2, 200, 8 + 96, 48, 24);
    lmove_size(go->tab3, go->adva_tab.cb1, 10, 10, 120, 24);
    lmove_size(go->tab3, go->adva_tab.combo1, 10, 40, width - 126 - 20, 24);
    lmove_size(go->tab3, go->adva_tab.cb2, 10, 74, 170, 24);
    lmove_size(go->fixed, go->tv1_scroll, 10, 200, width - 20, height - 225);
}

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
        LOGLN0((fi, LOG_DEBUG, LOGS "resized to %dx%d, was %dx%d", LOGP,
                levent->width, levent->height, go->width, go->height));
        go->width = levent->width;
        go->height = levent->height;
        lmove_size_width_height(go, go->width, go->height);
    }
    return FALSE;
}

/*****************************************************************************/
static void
init_list(GtkWidget* list)
{
    GtkCellRenderer* renderer;
    GtkTreeViewColumn* column;
    GtkListStore* store;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name",
                                                      renderer, "text",
                                                      0, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_set_sort_column_id(column, 0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("In Subfolder",
                                                      renderer, "text",
                                                      1, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_set_sort_column_id(column, 1);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Size",
                                                      renderer, "text",
                                                      2, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_set_sort_column_id(column, 2);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Modified",
                                                      renderer, "text",
                                                      3, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_set_sort_column_id(column, 3);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING,
                               G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

    g_object_unref(store);
}

/*****************************************************************************/
static void
add_to_list(GtkWidget* list, const gchar* name, const gchar* in_subfolder,
            const gchar* size, const gchar* modified)
{
    GtkListStore* store;
    GtkTreeIter iter;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, name, 1, in_subfolder, 2, size,
                       3, modified, -1);
}

/*****************************************************************************/
int
gui_object_create(struct finder_info* fi, struct gui_object** ago,
                  int argc, char** argv)
{
    struct gui_object* go;
    GdkGeometry geometry;
    GtkTreeSelection* selection;
    GtkAdjustment* adjustment;

    go = (struct gui_object*)calloc(1, sizeof(struct gui_object));
    go->fi = fi;
    gtk_init(&argc, &argv);
    go->mw = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(go->mw), "Finder");
    gtk_window_set_default_size(GTK_WINDOW(go->mw),
                                GUI_START_WIDTH, GUI_START_HEIGHT);
    gtk_widget_add_events(go->mw, GDK_CONFIGURE);

    go->fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(go->mw), go->fixed);

    go->but1 = gtk_button_new_with_label("Find");
    gtk_container_add(GTK_CONTAINER(go->fixed), go->but1);

    go->but2 = gtk_button_new_with_label("Stop");
    gtk_container_add(GTK_CONTAINER(go->fixed), go->but2);

    go->but3 = gtk_button_new_with_label("Exit");
    gtk_container_add(GTK_CONTAINER(go->fixed), go->but3);

    go->notebook = gtk_notebook_new();

    go->tab1 = gtk_fixed_new();
    go->tab1_label = gtk_label_new("Name/Location");
    gtk_notebook_append_page(GTK_NOTEBOOK(go->notebook),
                             go->tab1, go->tab1_label);

    go->name_tab.label1 = gtk_label_new("Named:");
    gtk_misc_set_alignment(GTK_MISC(go->name_tab.label1), 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(go->tab1), go->name_tab.label1);

    go->name_tab.combo1 = gtk_combo_box_new_with_entry();
    gtk_container_add(GTK_CONTAINER(go->tab1), go->name_tab.combo1);

    go->name_tab.label2 = gtk_label_new("Look in:");
    gtk_misc_set_alignment(GTK_MISC(go->name_tab.label2), 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(go->tab1), go->name_tab.label2);

    go->name_tab.combo2 = gtk_combo_box_new_with_entry();
    gtk_container_add(GTK_CONTAINER(go->tab1), go->name_tab.combo2);

    go->name_tab.but1 = gtk_button_new_with_label("Browse");
    gtk_container_add(GTK_CONTAINER(go->tab1), go->name_tab.but1);

    go->name_tab.cb1 = gtk_check_button_new_with_label("Include subfolders");
    gtk_container_add(GTK_CONTAINER(go->tab1), go->name_tab.cb1);

    go->name_tab.cb2 = gtk_check_button_new_with_label("Case sensitive search");
    gtk_container_add(GTK_CONTAINER(go->tab1), go->name_tab.cb2);

    go->name_tab.cb3 = gtk_check_button_new_with_label("Show hidden files");
    gtk_container_add(GTK_CONTAINER(go->tab1), go->name_tab.cb3);

    go->tab2 = gtk_fixed_new();
    go->tab2_label = gtk_label_new("Date Modified");
    gtk_notebook_append_page(GTK_NOTEBOOK(go->notebook),
                             go->tab2, go->tab2_label);

    go->date_tab.rb1 = gtk_radio_button_new_with_label(NULL, "All files");
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.rb1);

    go->date_tab.rb2 = gtk_radio_button_new_with_label_from_widget
        (GTK_RADIO_BUTTON(go->date_tab.rb1), "Find all files");
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.rb2);

    go->date_tab.combo1 = gtk_combo_box_new_with_entry();
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.combo1);

    go->date_tab.rb3 = gtk_radio_button_new_with_label(NULL, "between");
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.rb3);

    go->date_tab.rb4 = gtk_radio_button_new_with_label_from_widget
        (GTK_RADIO_BUTTON(go->date_tab.rb3), "during the previous");
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.rb4);

    go->date_tab.label1 = gtk_label_new("months(s)");
    gtk_misc_set_alignment(GTK_MISC(go->date_tab.label1), 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.label1);

    go->date_tab.rb5 = gtk_radio_button_new_with_label_from_widget
        (GTK_RADIO_BUTTON(go->date_tab.rb4), "during the previous");
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.rb5);

    go->date_tab.label2 = gtk_label_new("days(s)");
    gtk_misc_set_alignment(GTK_MISC(go->date_tab.label2), 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.label2);

    adjustment = GTK_ADJUSTMENT
        (gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 5.0, 0.0));
    go->date_tab.spinner1 = gtk_spin_button_new(adjustment, 1.0, 0);
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.spinner1);

    adjustment = GTK_ADJUSTMENT
        (gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 5.0, 0.0));
    go->date_tab.spinner2 = gtk_spin_button_new(adjustment, 1.0, 0);
    gtk_container_add(GTK_CONTAINER(go->tab2), go->date_tab.spinner2);

    go->tab3 = gtk_fixed_new();
    go->tab3_label = gtk_label_new("Advanced");
    gtk_notebook_append_page(GTK_NOTEBOOK(go->notebook),
                             go->tab3, go->tab3_label);

    go->adva_tab.cb1 = gtk_check_button_new_with_label("Search in files");
    gtk_container_add(GTK_CONTAINER(go->tab3), go->adva_tab.cb1);

    go->adva_tab.combo1 = gtk_combo_box_new_with_entry();
    gtk_container_add(GTK_CONTAINER(go->tab3),
                      go->adva_tab.combo1);

    go->adva_tab.cb2 = gtk_check_button_new_with_label("Case sensitive search");
    gtk_container_add(GTK_CONTAINER(go->tab3), go->adva_tab.cb2);

    go->tv1 = gtk_tree_view_new();
    go->tv1_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(go->tv1_scroll),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(go->fixed), go->tv1_scroll);
    gtk_container_add(GTK_CONTAINER(go->tv1_scroll), go->tv1);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(go->tv1), TRUE);
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(go->tv1));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
    init_list(go->tv1);
    add_to_list(go->tv1, "Aliens", "1", "2", "3");
    add_to_list(go->tv1, "Leon", "4", "5", "6");
    add_to_list(go->tv1, "The Verdict", "7", "8", "9");
    add_to_list(go->tv1, "North Face", "10", "11", "12");
    add_to_list(go->tv1, "Der Untergang", "13", "14", "15");

    gtk_container_add(GTK_CONTAINER(go->fixed), go->notebook);

    g_signal_connect(G_OBJECT(go->mw), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(go->mw), "configure-event",
                     G_CALLBACK(configure_callback), fi);

    memset(&geometry, 0, sizeof(geometry));
    geometry.min_width = 500;
    geometry.min_height = 300;
    geometry.max_width = 4096;
    geometry.max_height = 4096;
    gtk_window_set_geometry_hints(GTK_WINDOW(go->mw),
                                  go->mw, &geometry,
                                  GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE);

    lmove_size_width_height(go, GUI_START_WIDTH, GUI_START_HEIGHT);
    gtk_widget_show_all(go->mw);

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

