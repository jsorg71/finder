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
#include "finder_thread.h"
#include "finder_gui.h"
#include "finder_gui_about.h"
#include "finder_mainwindow.h"

#define GUI_START_WIDTH     640
#define GUI_START_HEIGHT    480
#define GUI_MIN_WIDTH       500
#define GUI_MIN_HEIGHT      300
#define GUI_MAX_WIDTH       4096
#define GUI_MAX_HEIGHT      4096

/*****************************************************************************/
static void
mw_remove_combo_box_str(GtkComboBoxText* cbt, const gchar* text)
{
    GtkTreeModel* tm;
    gint ncol;
    gint index;
    GtkTreeIter iter;
    GValue value1;
    const gchar* item_text;
    gboolean cont;

    if (text == NULL)
    {
        return;
    }
    tm = gtk_combo_box_get_model(GTK_COMBO_BOX(cbt));
    ncol = gtk_tree_model_get_n_columns(tm);
    cont = ncol > 0;
    while (cont)
    {
        cont = FALSE;
        index = 0;
        if (gtk_tree_model_get_iter_first(tm, &iter))
        {
            do
            {
                memset(&value1, 0, sizeof(value1));
                gtk_tree_model_get_value(tm, &iter, 0, &value1);
                item_text = g_value_get_string(&value1);
                if (item_text != NULL)
                {
                    if (finder_stricmp(item_text, text) == 0)
                    {
                        g_value_unset(&value1);
                        gtk_combo_box_text_remove(cbt, index);
                        cont = TRUE;
                        break;
                    }
                }
                g_value_unset(&value1);
                index++;
            }
            while (gtk_tree_model_iter_next(tm, &iter));
        }
    }
}

/*****************************************************************************/
static void
mw_combo_box_text_set_active_text(GtkComboBoxText* cbt, const gchar* text)
{
    GtkWidget* child;

    child = gtk_bin_get_child(GTK_BIN(cbt));
    gtk_entry_set_text(GTK_ENTRY(child), text);
}

/*****************************************************************************/
static void
mw_save_combo(struct gui_object* go, GtkComboBoxText* cbt,
              const gchar* section, const gchar* key)
{
    const gchar* text;
    struct finder_info* fi;

    (void)section;
    (void)key;

    fi = go->fi;
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    text = gtk_combo_box_text_get_active_text(cbt);
    mw_remove_combo_box_str(cbt, text);
    gtk_combo_box_text_prepend_text(cbt, text);
    gtk_combo_box_set_active(GTK_COMBO_BOX(cbt), 0);
}

/*****************************************************************************/
static void
mw_save_check_box(struct gui_object* go, GtkCheckButton* cb,
                  const gchar* section, const gchar* key,
                  gboolean default_checked)
{
    struct finder_info* fi;

    (void)cb;
    (void)section;
    (void)key;
    (void)default_checked;

    fi = go->fi;
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
}

/*****************************************************************************/
static void
mw_save_all(struct gui_object* go)
{
    struct finder_info* fi;
    GtkCheckButton* cb;
    GtkComboBoxText* cbt;

    fi = go->fi;
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    /* Name/Location tab */
    cbt = GTK_COMBO_BOX_TEXT(go->name_tab.combo1);
    mw_save_combo(go, cbt, "NameLocation", "Named");
    cbt = GTK_COMBO_BOX_TEXT(go->name_tab.combo2);
    mw_save_combo(go, cbt, "NameLocation", "LookIn");
    cb = GTK_CHECK_BUTTON(go->name_tab.cb1);
    mw_save_check_box(go, cb, "NameLocation", "IncludeSubfolders", TRUE);
    cb = GTK_CHECK_BUTTON(go->name_tab.cb2);
    mw_save_check_box(go, cb, "NameLocation", "CaseSensitiveSearch", FALSE);
    cb = GTK_CHECK_BUTTON(go->name_tab.cb3);
    mw_save_check_box(go, cb, "NameLocation", "ShowHiddenFiles", FALSE);
    /* Advanced tab */
    cb = GTK_CHECK_BUTTON(go->adva_tab.cb1);
    mw_save_check_box(go, cb, "Advanced", "SearchInFiles", FALSE);
    cbt = GTK_COMBO_BOX_TEXT(go->adva_tab.combo1);
    mw_save_combo(go, cbt, "Advanced", "SearchInFilesText");
    cb = GTK_CHECK_BUTTON(go->adva_tab.cb2);
    mw_save_check_box(go, cb, "Advanced", "CaseSensitiveSearch", FALSE);
}

/*****************************************************************************/
static void
mw_find_button_clicked(GtkWidget *widget, gpointer data)
{
    struct gui_object* go;
    struct finder_info* fi;
    GtkTreeModel* tm;
    GtkListStore* store;
    GtkComboBoxText* cbt;
    GtkToggleButton* tb;
    gchar* text;
    gboolean checked;

    fi = (struct finder_info*)data;
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    if (widget == go->but1)
    {
        LOGLN0((fi, LOG_INFO, LOGS "Find clicked", LOGP));
        /* named */
        cbt = GTK_COMBO_BOX_TEXT(go->name_tab.combo1);
        text = gtk_combo_box_text_get_active_text(cbt);
        finder_snprintf(fi->named, sizeof(fi->named), "%s", text);
        /* look in */
        cbt = GTK_COMBO_BOX_TEXT(go->name_tab.combo2);
        text = gtk_combo_box_text_get_active_text(cbt);
        finder_snprintf(fi->look_in, sizeof(fi->look_in), "%s", text);
        /* include subfolders */
        tb = GTK_TOGGLE_BUTTON(go->name_tab.cb1);
        checked = gtk_toggle_button_get_active(tb);
        fi->include_subfolders = checked;
        /* case sesitive */
        tb = GTK_TOGGLE_BUTTON(go->name_tab.cb2);
        checked = gtk_toggle_button_get_active(tb);
        fi->case_sensitive = checked;
        /* hidden */
        tb = GTK_TOGGLE_BUTTON(go->name_tab.cb3);
        checked = gtk_toggle_button_get_active(tb);
        fi->show_hidden = checked;
        /* sarch in files */
        tb = GTK_TOGGLE_BUTTON(go->adva_tab.cb1);
        checked = gtk_toggle_button_get_active(tb);
        fi->search_in_files = checked;
        /* search in text */
        cbt = GTK_COMBO_BOX_TEXT(go->adva_tab.combo1);
        text = gtk_combo_box_text_get_active_text(cbt);
        finder_snprintf(fi->text, sizeof(fi->text), "%s", text);
        /* search in case sensitive */
        tb = GTK_TOGGLE_BUTTON(go->adva_tab.cb2);
        checked = gtk_toggle_button_get_active(tb);
        fi->search_in_case_sensitive = checked;
        /* clear list */
        tm = gtk_tree_view_get_model(GTK_TREE_VIEW(go->tv1));
        store = GTK_LIST_STORE(tm);
        gtk_list_store_clear(store);

        mw_save_all(go);

        /* start search */
        start_find(fi);

        gtk_widget_set_sensitive(go->but1, FALSE); /* disable Find button */
        gtk_widget_set_sensitive(go->but2, TRUE); /* enable Stop button */
    }
}

/*****************************************************************************/
static void
mw_stop_button_clicked(GtkWidget *widget, gpointer data)
{
    struct gui_object* go;
    struct finder_info* fi;

    fi = (struct finder_info*)data;
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    if (widget == go->but2)
    {
        stop_find(fi);
    }
}

/*****************************************************************************/
static void
mw_browse_button_clicked(GtkWidget *widget, gpointer data)
{
    struct gui_object* go;
    struct finder_info* fi;

    GtkWidget* dialog;
    GtkFileChooserAction action;
    gint res;
    gchar* filename;
    GtkFileChooser* chooser;
    GtkComboBoxText* cbt;

    fi = (struct finder_info*)data;
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    if (widget == go->name_tab.but1)
    {
        LOGLN0((fi, LOG_INFO, LOGS "Browse clicked", LOGP));
        action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
        dialog = gtk_file_chooser_dialog_new
            ("Select Look In directory", GTK_WINDOW(go->mw), action,
             "_Cancel", GTK_RESPONSE_CANCEL,
             "_Ok", GTK_RESPONSE_ACCEPT, NULL);
        chooser = GTK_FILE_CHOOSER(dialog);
        cbt = GTK_COMBO_BOX_TEXT(go->name_tab.combo2);
        filename = gtk_combo_box_text_get_active_text(cbt);
        if (filename != NULL)
        {
            gtk_file_chooser_set_current_folder(chooser, filename);
            g_free(filename);
        }
        res = gtk_dialog_run(GTK_DIALOG(dialog));
        if (res == GTK_RESPONSE_ACCEPT)
        {
            filename = gtk_file_chooser_get_filename(chooser);
            if (filename != NULL)
            {
                LOGLN0((fi, LOG_INFO, LOGS "Ok clicked, filename %s",
                        LOGP, filename));
                cbt = GTK_COMBO_BOX_TEXT(go->name_tab.combo2);
                mw_combo_box_text_set_active_text(cbt, filename);
                g_free(filename);
            }
        }
        else
        {
            LOGLN0((fi, LOG_INFO, LOGS "Cancel clicked", LOGP));
        }
        gtk_widget_destroy(dialog);
    }
}

/*****************************************************************************/
static void
mw_move_size(GtkWidget* fixed, GtkWidget* widget,
             int x, int y, int cx, int cy)
{
    gtk_fixed_move(GTK_FIXED(fixed), widget, x, y);
    if ((cx > 0) && (cy > 0))
    {
        gtk_widget_set_size_request(widget, cx, cy);
    }
}

/*****************************************************************************/
static void
mw_move_size_width_height(struct gui_object* go, int width, int height)
{
    mw_move_size(go->mw_fixed, go->menubar, 0, 0, width, 24);
    mw_move_size(go->mw_fixed, go->but1, width - 110, 32, 100, 30);
    mw_move_size(go->mw_fixed, go->but2, width - 110, 72, 100, 30);
    mw_move_size(go->mw_fixed, go->but3, width - 110, 165, 100, 30);
    mw_move_size(go->mw_fixed, go->notebook, 8, 28, width - 126, 168);
    mw_move_size(go->tab1_fixed, go->name_tab.label1, 8, 8, -1, -1);
    mw_move_size(go->tab1_fixed, go->name_tab.combo1, 85, 8, width - 126 - 100, 24);
    mw_move_size(go->tab1_fixed, go->name_tab.label2, 8, 41, -1, -1);
    mw_move_size(go->tab1_fixed, go->name_tab.combo2, 85, 41, width - 126 - 165, 24);
    mw_move_size(go->tab1_fixed, go->name_tab.but1, width - 126 - 75, 41, 60, 24);
    mw_move_size(go->tab1_fixed, go->name_tab.cb1, 8, 74, -1, -1);
    mw_move_size(go->tab1_fixed, go->name_tab.cb2, 8, 107, -1, -1);
    mw_move_size(go->tab1_fixed, go->name_tab.cb3, 216, 74, -1, -1);
    mw_move_size(go->tab2_fixed, go->date_tab.rb1, 10, 8, -1, -1);
    mw_move_size(go->tab2_fixed, go->date_tab.rb2, 10, 8 + 24, -1, -1);
    mw_move_size(go->tab2_fixed, go->date_tab.combo1, 110, 8 + 24, 100, 24);
    mw_move_size(go->tab2_fixed, go->date_tab.rb3, 30, 8 + 48, -1, -1);
    mw_move_size(go->tab2_fixed, go->date_tab.rb4, 30, 8 + 72, -1, -1);
    mw_move_size(go->tab2_fixed, go->date_tab.label1, 250, 8 + 72, -1, -1);
    mw_move_size(go->tab2_fixed, go->date_tab.rb5, 30, 8 + 96, -1, -1);
    mw_move_size(go->tab2_fixed, go->date_tab.label2, 250, 8 + 96, -1, -1);
    mw_move_size(go->tab2_fixed, go->date_tab.spinner1, 200, 8 + 72, 48, 24);
    mw_move_size(go->tab2_fixed, go->date_tab.spinner2, 200, 8 + 96, 48, 24);
    mw_move_size(go->tab3_fixed, go->adva_tab.cb1, 10, 10, -1, -1);
    mw_move_size(go->tab3_fixed, go->adva_tab.combo1, 10, 40, width - 126 - 20, 24);
    mw_move_size(go->tab3_fixed, go->adva_tab.cb2, 10, 74, -1, -1);
    mw_move_size(go->mw_fixed, go->tv1_scroll, 10, 200, width - 20, height - 225);
    mw_move_size(go->mw_fixed, go->sb, 0, height - 24, width - 200, 24);
    mw_move_size(go->mw_fixed, go->sb1, width - 200, height - 24, 200, 24);
}

/*****************************************************************************/
static gboolean
mw_configure_callback(GtkWindow* window, GdkEvent* event, gpointer data)
{
    struct gui_object* go;
    struct finder_info* fi;
    GdkEventConfigure* levent;

    levent = (GdkEventConfigure*)event;
    fi = (struct finder_info*)data;
    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    if (GTK_WIDGET(window) != go->mw)
    {
        return FALSE;
    }
    if ((go->width != levent->width) || (go->height != levent->height))
    {
        LOGLN10((fi, LOG_DEBUG, LOGS "resized to %dx%d, was %dx%d", LOGP,
                 levent->width, levent->height, go->width, go->height));
        go->width = levent->width;
        go->height = levent->height;
        mw_move_size_width_height(go, go->width, go->height);
    }
    return FALSE;
}

/*****************************************************************************/
static void
mw_list_add_column(GtkWidget* list, const gchar* col_name,
                   int display_index, int sort_index)
{
    GtkCellRenderer* renderer;
    GtkTreeViewColumn* column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes
        (col_name, renderer, "text", display_index, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_set_sort_column_id(column, sort_index);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
}

/*****************************************************************************/
static void
mw_init_list(GtkWidget* list)
{
    GtkListStore* store;

    store = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING,
                               G_TYPE_INT64, G_TYPE_STRING,
                               G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));
    g_object_unref(store);
    mw_list_add_column(list, "Name", 0, 0);
    mw_list_add_column(list, "In Subfolder", 1, 1);
    mw_list_add_column(list, "Size", 3, 2);
    mw_list_add_column(list, "Modified", 4, 4);
}

/*****************************************************************************/
static void
mw_add_to_list(GtkWidget* list, const gchar* name, const gchar* in_subfolder,
               FINDER_I64 size, const gchar* modified)
{
    GtkListStore* store;
    GtkTreeIter iter;
    gchar size_name[64];

    if (format_commas(size, size_name, sizeof(size_name)) != 0)
    {
        size_name[0] = '0';
        size_name[1] = 0;
    }
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, name, 1, in_subfolder,
                       2, size, 3, size_name, 4, modified, -1);
}

/*****************************************************************************/
int
mw_create(struct finder_info* fi, struct gui_object** ago,
          int argc, char** argv)
{
    struct gui_object* go;
    GdkGeometry geometry;
    GtkTreeSelection* selection;
    GtkAdjustment* adjustment;

    go = g_new0(struct gui_object, 1);
    go->fi = fi;
    gtk_init(&argc, &argv);
    go->mw = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(go->mw), "Finder");
    gtk_window_set_default_size(GTK_WINDOW(go->mw),
                                GUI_START_WIDTH, GUI_START_HEIGHT);
    gtk_widget_add_events(go->mw, GDK_CONFIGURE);

    go->mw_fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(go->mw), go->mw_fixed);

    go->but1 = gtk_button_new_with_label("F_ind");
    gtk_button_set_use_underline(GTK_BUTTON(go->but1), TRUE);
    gtk_container_add(GTK_CONTAINER(go->mw_fixed), go->but1);

    go->but2 = gtk_button_new_with_label("_Stop");
    gtk_button_set_use_underline(GTK_BUTTON(go->but2), TRUE);
    gtk_widget_set_sensitive(go->but2, FALSE);
    gtk_container_add(GTK_CONTAINER(go->mw_fixed), go->but2);

    go->but3 = gtk_button_new_with_label("Exit");
    gtk_container_add(GTK_CONTAINER(go->mw_fixed), go->but3);

    go->notebook = gtk_notebook_new();

    go->tab1_fixed = gtk_fixed_new();
    go->tab1_label = gtk_label_new("Name/_Location");
    gtk_label_set_use_underline(GTK_LABEL(go->tab1_label), TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(go->notebook),
                             go->tab1_fixed, go->tab1_label);

    go->name_tab.label1 = gtk_label_new("Named:");
    gtk_misc_set_alignment(GTK_MISC(go->name_tab.label1), 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(go->tab1_fixed), go->name_tab.label1);

    go->name_tab.combo1 = gtk_combo_box_text_new_with_entry();
    gtk_container_add(GTK_CONTAINER(go->tab1_fixed), go->name_tab.combo1);

    go->name_tab.label2 = gtk_label_new("Look in:");
    gtk_misc_set_alignment(GTK_MISC(go->name_tab.label2), 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(go->tab1_fixed), go->name_tab.label2);

    go->name_tab.combo2 = gtk_combo_box_text_new_with_entry();
    gtk_container_add(GTK_CONTAINER(go->tab1_fixed), go->name_tab.combo2);

    go->name_tab.but1 = gtk_button_new_with_label("_Browse");
    gtk_button_set_use_underline(GTK_BUTTON(go->name_tab.but1), TRUE);
    gtk_container_add(GTK_CONTAINER(go->tab1_fixed), go->name_tab.but1);

    go->name_tab.cb1 = gtk_check_button_new_with_label("Include subfolders");
    gtk_container_add(GTK_CONTAINER(go->tab1_fixed), go->name_tab.cb1);

    go->name_tab.cb2 = gtk_check_button_new_with_label
        ("Case sensitive search");
    gtk_container_add(GTK_CONTAINER(go->tab1_fixed), go->name_tab.cb2);

    go->name_tab.cb3 = gtk_check_button_new_with_label("Show hidden files");
    gtk_container_add(GTK_CONTAINER(go->tab1_fixed), go->name_tab.cb3);

    go->tab2_fixed = gtk_fixed_new();
    go->tab2_label = gtk_label_new("_Date Modified");
    gtk_label_set_use_underline(GTK_LABEL(go->tab2_label), TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(go->notebook),
                             go->tab2_fixed, go->tab2_label);

    go->date_tab.rb1 = gtk_radio_button_new_with_label(NULL, "All files");
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.rb1);

    go->date_tab.rb2 = gtk_radio_button_new_with_label_from_widget
        (GTK_RADIO_BUTTON(go->date_tab.rb1), "Find all files");
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.rb2);

    go->date_tab.combo1 = gtk_combo_box_text_new_with_entry();
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.combo1);

    go->date_tab.rb3 = gtk_radio_button_new_with_label(NULL, "between");
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.rb3);

    go->date_tab.rb4 = gtk_radio_button_new_with_label_from_widget
        (GTK_RADIO_BUTTON(go->date_tab.rb3), "during the previous");
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.rb4);

    go->date_tab.label1 = gtk_label_new("months(s)");
    gtk_misc_set_alignment(GTK_MISC(go->date_tab.label1), 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.label1);

    go->date_tab.rb5 = gtk_radio_button_new_with_label_from_widget
        (GTK_RADIO_BUTTON(go->date_tab.rb4), "during the previous");
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.rb5);

    go->date_tab.label2 = gtk_label_new("days(s)");
    gtk_misc_set_alignment(GTK_MISC(go->date_tab.label2), 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.label2);

    adjustment = GTK_ADJUSTMENT
        (gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 5.0, 0.0));
    go->date_tab.spinner1 = gtk_spin_button_new(adjustment, 1.0, 0);
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.spinner1);

    adjustment = GTK_ADJUSTMENT
        (gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 5.0, 0.0));
    go->date_tab.spinner2 = gtk_spin_button_new(adjustment, 1.0, 0);
    gtk_container_add(GTK_CONTAINER(go->tab2_fixed), go->date_tab.spinner2);

    go->tab3_fixed = gtk_fixed_new();
    go->tab3_label = gtk_label_new("_Advanced");
    gtk_label_set_use_underline(GTK_LABEL(go->tab3_label), TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(go->notebook),
                             go->tab3_fixed, go->tab3_label);

    go->adva_tab.cb1 = gtk_check_button_new_with_label("Search in files");
    gtk_container_add(GTK_CONTAINER(go->tab3_fixed), go->adva_tab.cb1);

    go->adva_tab.combo1 = gtk_combo_box_text_new_with_entry();
    gtk_container_add(GTK_CONTAINER(go->tab3_fixed),
                      go->adva_tab.combo1);

    go->adva_tab.cb2 = gtk_check_button_new_with_label
        ("Case sensitive search");
    gtk_container_add(GTK_CONTAINER(go->tab3_fixed), go->adva_tab.cb2);

    go->tv1 = gtk_tree_view_new();
    go->tv1_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(go->tv1_scroll),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(go->mw_fixed), go->tv1_scroll);
    gtk_container_add(GTK_CONTAINER(go->tv1_scroll), go->tv1);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(go->tv1), TRUE);
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(go->tv1));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
    mw_init_list(go->tv1);

    gtk_container_add(GTK_CONTAINER(go->mw_fixed), go->notebook);

    go->menubar = gtk_menu_bar_new();
    go->file_menu = gtk_menu_new();
    go->help_menu = gtk_menu_new();
    go->file_mi = gtk_menu_item_new_with_label("File");
    go->quit_mi = gtk_menu_item_new_with_label("Exit");
    go->help_mi = gtk_menu_item_new_with_label("Help");
    go->helpDDD_mi = gtk_menu_item_new_with_label("Help...");
    go->about_mi = gtk_menu_item_new_with_label("About");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(go->file_mi), go->file_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(go->help_mi), go->help_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(go->file_menu), go->quit_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(go->help_menu), go->helpDDD_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(go->help_menu), go->about_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(go->menubar), go->file_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(go->menubar), go->help_mi);
    gtk_container_add(GTK_CONTAINER(go->mw_fixed), go->menubar);

    go->sb = gtk_statusbar_new();
    gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(go->sb), FALSE);
    gtk_container_add(GTK_CONTAINER(go->mw_fixed), go->sb);

    go->sb1 = gtk_statusbar_new();
    gtk_container_add(GTK_CONTAINER(go->mw_fixed), go->sb1);

    gtk_statusbar_push(GTK_STATUSBAR(go->sb), 0, "Ready.");
    gtk_statusbar_push(GTK_STATUSBAR(go->sb1), 0, "");

    g_signal_connect(G_OBJECT(go->mw), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(go->mw), "configure-event",
                     G_CALLBACK(mw_configure_callback), fi);

    //g_signal_connect(G_OBJECT(go->quitMi), "activate",
    //                 G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(G_OBJECT(go->name_tab.but1), "clicked",
                     G_CALLBACK(mw_browse_button_clicked), fi);

    g_signal_connect(G_OBJECT(go->but1), "clicked",
                     G_CALLBACK(mw_find_button_clicked), fi);

    g_signal_connect(G_OBJECT(go->but2), "clicked",
                     G_CALLBACK(mw_stop_button_clicked), fi);

    memset(&geometry, 0, sizeof(geometry));
    geometry.min_width = GUI_MIN_WIDTH;
    geometry.min_height = GUI_MIN_HEIGHT;
    geometry.max_width = GUI_MAX_WIDTH;
    geometry.max_height = GUI_MAX_HEIGHT;
    gtk_window_set_geometry_hints(GTK_WINDOW(go->mw),
                                  go->mw, &geometry,
                                  GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE);
    finder_event_create(&(go->gui_event));
    finder_mutex_create(&(go->mutex1));
    mw_move_size_width_height(go, GUI_START_WIDTH, GUI_START_HEIGHT);
    gtk_widget_show_all(go->mw);
    *ago = go;
    return 0;
}

/*****************************************************************************/
static gboolean
mw_event_func(GIOChannel *source, GIOCondition condition, gpointer data)
{
    struct finder_info* fi;
    struct gui_object* go;

    (void)source;
    (void)condition;
    go = (struct gui_object*)data;
    fi = go->fi;
    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    finder_event_clear(go->gui_event);
    event_callback(fi);
    return TRUE;
}

/*****************************************************************************/
int
mw_main_loop(struct gui_object* go)
{
    GIOChannel* chan;
    int fd;

    gui_init(go->fi);
    fd = finder_event_get_wait_obj(go->gui_event);
    chan = g_io_channel_unix_new(fd);
    g_io_add_watch(chan, G_IO_IN, mw_event_func, go);
    gtk_main();
    gui_deinit(go->fi);
    return 0;
}

/*****************************************************************************/
int
mw_delete(struct gui_object* go)
{
    finder_mutex_delete(go->mutex1);
    finder_event_delete(go->gui_event);
    g_free(go);
    return 0;
}

/*****************************************************************************/
int
mw_set_event(struct gui_object* go)
{
    LOGLN10((go->fi, LOG_INFO, LOGS, LOGP));
    finder_mutex_lock(go->mutex1);
    finder_event_set(go->gui_event);
    finder_mutex_unlock(go->mutex1);
    return 0;
}

/*****************************************************************************/
int
mw_find_done(struct gui_object* go)
{
    struct finder_info* fi;
    GtkTreeModel* tm;
    gint count;
    gchar text[64];

    fi = go->fi;
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    gtk_widget_set_sensitive(go->but1, TRUE); /* enable Find button */
    gtk_widget_set_sensitive(go->but2, FALSE); /* disable Stop button */
    tm = gtk_tree_view_get_model(GTK_TREE_VIEW(go->tv1));
    count = gtk_tree_model_iter_n_children(tm, NULL);
    finder_snprintf(text, sizeof(text), "%d Items found", count);
    gtk_statusbar_push(GTK_STATUSBAR(go->sb1), 0, text);
    return 0;
}

/*****************************************************************************/
int
mw_add_one(struct gui_object* go, const char* filename,
           const char* in_subfolder, FINDER_I64 size,
           const char* modified)
{
    LOGLN10((go->fi, LOG_INFO, LOGS, LOGP));
    mw_add_to_list(go->tv1, filename, in_subfolder, size, modified);
    return 0;
}

