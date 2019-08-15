/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2019
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

/* for IMalloc_Release and IMalloc_Free used with SHBrowseForFolder */
#define COBJMACROS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>

#include "finder.h"
#include "finder_event.h"

#define TAB_NUM_TABS 3

struct gui_object
{
    HINSTANCE hInstance;
    HWND hwnd;
    HWND hwndFindButton;
    HWND hwndStopButton;
    HWND hwndExitButton;
    HWND hwndBrowseButton;
    HWND hwndListView;
    HWND hwndTabControl;
    HWND hwndTabs[TAB_NUM_TABS];
    HWND hwndNamedLabel;
    HWND hwndNamedEdit;
    HWND hwndLookInLabel;
    HWND hwndLookInEdit;
    HWND hwndSubfolderCB;
    HWND hwndCaseSensativeCB;
    HWND hwndHiddenCB;
    HWND hwndSearchInFileCB;
    HWND hwndSearchInFileEdit;
    HWND hwndCaseSensativeSearchCB;
    HWND hwndStatusBar;
    HMENU hMenubar;
    HMENU hMenuFile;
    HMENU hMenuHelp;
    WNDPROC hwndTabs0WndProcOrg;

    HANDLE event;
    HFONT font;
    DWORD startup_timer;
    int sort_order[4];
};

struct lv_item
{
    char* filename;
    char* in_subfolder;
    FINDER_I64 size;
    char* size_text;
    char* modified;
};

static const char g_class_name[] = "Finder Window Class";

static LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define GET_FI_FROM_WND(_hwnd) \
    (struct finder_info*)GetWindowLongPtr(_hwnd, GWLP_USERDATA)
#define SET_FI_TO_WND(_hwnd, _fi) \
    SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)_fi);

/*****************************************************************************/
int
gui_set_event(struct finder_info* fi)
{
    struct gui_object* go;

    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    SetEvent(go->event);
    return 0;
}

/*****************************************************************************/
int
gui_find_done(struct finder_info* fi)
{
    struct gui_object* go;
    int count;
    char text[64];

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    ListView_SetColumnWidth(go->hwndListView, 0, LVSCW_AUTOSIZE_USEHEADER);
    ListView_SetColumnWidth(go->hwndListView, 1, LVSCW_AUTOSIZE_USEHEADER);
    ListView_SetColumnWidth(go->hwndListView, 2, LVSCW_AUTOSIZE_USEHEADER);
    ListView_SetColumnWidth(go->hwndListView, 3, LVSCW_AUTOSIZE_USEHEADER);
    EnableWindow(go->hwndFindButton, TRUE);
    EnableWindow(go->hwndStopButton, FALSE);
    count = ListView_GetItemCount(go->hwndListView);
    finder_snprintf(text, 64, "%d items found", count);
    SendMessage(go->hwndStatusBar, SB_SETTEXT, 1, (LPARAM)text);
    return 0;
}

/*****************************************************************************/
int
gui_add_one(struct finder_info* fi, const char* filename,
            const char* in_subfolder, FINDER_I64 size,
            const char* modified)
{
    LV_ITEM item;
    struct gui_object* go;
    struct lv_item* lvi;

    go = (struct gui_object*)(fi->gui_obj);
    LOGLN10((fi, LOG_INFO, LOGS "filename [%s] subfolder [%s] size "
            "%" FINDER_PRId64 " modified [%s]", LOGP, filename, in_subfolder,
            size, modified));

    /* allocate */
    while (1)
    {
        lvi = (struct lv_item*)calloc(1, sizeof(struct lv_item));
        if (lvi != NULL)
        {
            lvi->size_text = (char*)calloc(1, 256);
            if (lvi->size_text != NULL)
            {
                lvi->filename = finder_strdup(filename);
                lvi->in_subfolder = finder_strdup(in_subfolder);
                lvi->modified = finder_strdup(modified);
                break;
            }
            free(lvi);
        }
        return 0;
    }
    lvi->size = size;
    format_commas(size, lvi->size_text, 256);

    memset(&item, 0, sizeof(item));
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.pszText = lvi->filename;
    item.cchTextMax = FINDER_STRLEN(item.pszText) + 1;
    item.lParam = (LONG_PTR)lvi;
    item.iItem = ListView_GetItemCount(go->hwndListView);
    ListView_InsertItem(go->hwndListView, &item);

    item.mask = LVIF_TEXT;
    item.iSubItem = 1;
    item.pszText = lvi->in_subfolder;
    item.cchTextMax = FINDER_STRLEN(item.pszText) + 1;
    item.lParam = 0;
    ListView_SetItem(go->hwndListView, &item);

    item.iSubItem = 2;
    item.pszText = lvi->size_text;
    item.cchTextMax = FINDER_STRLEN(item.pszText) + 1;
    ListView_SetItem(go->hwndListView, &item);

    item.iSubItem = 3;
    item.pszText = lvi->modified;
    item.cchTextMax = FINDER_STRLEN(item.pszText) + 1;
    ListView_SetItem(go->hwndListView, &item);

    return 0;
}

/*****************************************************************************/
int
gui_writeln(struct finder_info* fi, const char* msg)
{
    (void)fi;
    OutputDebugString(msg);
    return 0;
}

/*****************************************************************************/
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;
    MSG msg;
    int handle_count;
    int cont;
    HANDLE handles[16];
    struct gui_object* go;
    INITCOMMONCONTROLSEX icex;
    struct finder_info* fi;
    ATOM atom;
    NONCLIENTMETRICS non_client_metrics;

    (void)hPrevInstance;
    (void)lpCmdLine;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = g_class_name;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    atom = RegisterClass(&wc);
    if (atom == 0)
    {
        return 0;
    }
    /* init common controls */
    memset(&icex, 0, sizeof(icex));
    icex.dwSize = sizeof(icex);
    /* ICC_WIN95_CLASSES
       Load animate control, header, hot key, list-view, progress bar,
       status bar, tab, tooltip, toolbar, trackbar, tree-view, and
       up-down control classes. */
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);
    fi = (struct finder_info*)calloc(1, sizeof(struct finder_info));
    if (fi == NULL)
    {
        UnregisterClass(g_class_name, hInstance);
        return 0;
    }
    if (gui_init(fi) != 0)
    {
        free(fi);
        UnregisterClass(g_class_name, hInstance);
        return 0;
    }
    go = (struct gui_object*)calloc(1, sizeof(struct gui_object));
    if (go == NULL)
    {
        free(fi);
        UnregisterClass(g_class_name, hInstance);
        return 0;
    }
    go->event = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (go->event == NULL)
    {
        free(go);
        free(fi);
        UnregisterClass(g_class_name, hInstance);
        return 0;
    }
    go->hInstance = hInstance;
    fi->gui_obj = go;
    go->hwnd = CreateWindow(g_class_name, "Finder", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, NULL, hInstance, NULL);
    if (go->hwnd == NULL)
    {
        CloseHandle(go->event);
        free(go);
        free(fi);
        UnregisterClass(g_class_name, hInstance);
        return 0;
    }
    SET_FI_TO_WND(go->hwnd, fi);
    /* create font */
    memset(&non_client_metrics, 0, sizeof(non_client_metrics));
    non_client_metrics.cbSize = sizeof(non_client_metrics);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &non_client_metrics, 0);
    go->font = CreateFontIndirect(&non_client_metrics.lfMessageFont);
    /* show main window */
    ShowWindow(go->hwnd, nCmdShow);
    /* Run the message loop. */
    cont = 1;
    while (cont)
    {
        handle_count = 0;
        handles[handle_count++] = go->event;
        MsgWaitForMultipleObjects(handle_count, handles, FALSE,
                                  INFINITE, QS_ALLINPUT);
        if (WaitForSingleObject(go->event, 0) == 0)
        {
            ResetEvent(go->event);
            event_callback(fi);
        }
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                LOGLN0((fi, LOG_INFO, LOGS "got quit", LOGP));
                cont = 0;
                break;
            }
            if (!IsDialogMessage(go->hwnd, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    /* cleanup */
    gui_deinit(fi);
    DestroyWindow(go->hwnd);
    DeleteObject(go->font);
    CloseHandle(go->event);
    free(go);
    free(fi);
    UnregisterClass(g_class_name, hInstance);
    return 0;
}

/*****************************************************************************/
static int
get_fi_go_from_hwnd(HWND hwnd, struct finder_info** fi, struct gui_object** go)
{
    struct finder_info* lfi;
    struct gui_object* lgo;

    lfi = GET_FI_FROM_WND(hwnd);
    if (lfi == NULL)
    {
        return 1;
    }
    lgo = (struct gui_object*)(lfi->gui_obj);
    if (lgo == NULL)
    {
        return 2;
    }
    if (fi != NULL)
    {
        *fi = lfi;
    }
    if (go != NULL)
    {
        *go = lgo;
    }
    return 0;
}

/*****************************************************************************/
static int
finder_save_combo(struct finder_info* fi, HKEY hKey, HWND hwnd,
                  const char* section, const char* key_prefix)
{
    LONG lRes;
    HKEY hSectionKey;
    DWORD key_bytes;
    DWORD key_type;
    char text[256];
    char key_name[256];
    int index;
    int count;
    int cb_text_bytes;
    int cb_text_bytes1;
    char* cb_text;

    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    lRes = RegCreateKeyEx(hKey, section, 0, NULL, 0,
                          KEY_READ | KEY_WRITE, NULL, &hSectionKey, NULL);
    if (lRes != ERROR_SUCCESS)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "RegCreateKeyEx failed", LOGP));
        return 1;
    }
    if (!GetWindowText(hwnd, text, 255))
    {
        text[0] = 0;
    }
    LOGLN10((fi, LOG_INFO, LOGS "for edit [%s] got text [%s]", LOGP, key_prefix, text));
    if (text[0] != 0)
    {
        index = ComboBox_FindString(hwnd, 0, text);
        if (index == CB_ERR)
        {
            (void)ComboBox_InsertString(hwnd, 0, text);
        }
        else if (index > 0)
        {
            (void)ComboBox_DeleteString(hwnd, index);
            (void)ComboBox_InsertString(hwnd, 0, text);
        }
        (void)ComboBox_SetCurSel(hwnd, 0);
    }
    for (index = 0; index < 100; index++)
    {
        finder_snprintf(key_name, 255, "%s%2.2d", key_prefix, index);
        key_bytes = 0;
        lRes = RegQueryValueEx(hSectionKey, key_name, NULL, &key_type, NULL,
                               &key_bytes);
        if (lRes == ERROR_SUCCESS)
        {
            LOGLN10((fi, LOG_INFO, LOGS "for section [%s], found key name [%s], deleting", LOGP, section, key_name));
            lRes = RegDeleteValue(hSectionKey, key_name);
            if (lRes != ERROR_SUCCESS)
            {
                LOGLN0((fi, LOG_ERROR, LOGS "RegDeleteValue failed error 0x%8.8x", LOGP, lRes));
            }
        }
    }
    count = ComboBox_GetCount(hwnd);
    LOGLN10((fi, LOG_INFO, LOGS "for edit [%s] got count %d", LOGP, key_prefix, count));
    if (count != CB_ERR)
    {
        if (count > 100)
        {
            count = 100;
        }
        for (index = 0; index < count; index++)
        {
            cb_text_bytes = ComboBox_GetLBTextLen(hwnd, index);
            if (cb_text_bytes != CB_ERR)
            {
                if (cb_text_bytes > 0)
                {
                    cb_text = (char*)malloc(cb_text_bytes + 1);
                    if (cb_text != NULL)
                    {
                        cb_text_bytes1 = ComboBox_GetLBText(hwnd, index, cb_text);
                        if (cb_text_bytes == cb_text_bytes1)
                        {
                            finder_snprintf(key_name, 255, "%s%2.2d", key_prefix, index);
                            LOGLN10((fi, LOG_INFO, LOGS "for section [%s], found key name [%s], adding [%s]", LOGP, section, key_name, cb_text));
                            lRes = RegSetValueEx(hSectionKey, key_name, 0, REG_SZ, cb_text, cb_text_bytes + 1);
                            if (lRes != ERROR_SUCCESS)
                            {
                                LOGLN0((fi, LOG_ERROR, LOGS "RegSetValueEx failed", LOGP));
                            }
                        }
                        else
                        {
                            LOGLN0((fi, LOG_ERROR, LOGS "error CB_GETLBTEXT result does not match CB_GETLBTEXTLEN result", LOGP));
                        }
                        free(cb_text);
                    }
                    else
                    {
                        LOGLN0((fi, LOG_ERROR, LOGS "error malloc failed", LOGP));
                    }
                }
                else
                {
                    LOGLN0((fi, LOG_INFO, LOGS "error cb_text_bytes is zero", LOGP));
                }
            }
            else
            {
                LOGLN0((fi, LOG_ERROR, LOGS "error CB_GETLBTEXTLEN failed", LOGP));
            }
        }
    }
    RegCloseKey(hSectionKey);
    return 0;
}

/*****************************************************************************/
static int
finder_save_checkbox(struct finder_info* fi, HKEY hKey, HWND hwnd,
                     const char* section, const char* key_prefix, BOOL def)
{
    LONG lRes;
    HKEY hSectionKey;
    BOOL is_checked;
    DWORD key_value;
    DWORD key_type;

    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    lRes = RegCreateKeyEx(hKey, section, 0, NULL, 0,
                          KEY_READ | KEY_WRITE, NULL, &hSectionKey, NULL);
    if (lRes != ERROR_SUCCESS)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "RegCreateKeyEx failed", LOGP));
        return 1;
    }

    is_checked = Button_GetCheck(hwnd);
    if ((!is_checked) == (!def))
    {
        lRes = RegQueryValueEx(hSectionKey, key_prefix, NULL, &key_type, NULL, NULL);
        if (lRes == ERROR_SUCCESS)
        {
            lRes = RegDeleteValue(hSectionKey, key_prefix);
            if (lRes != ERROR_SUCCESS)
            {
                LOGLN0((fi, LOG_ERROR, LOGS "RegDeleteValue failed error 0x%8.8x", LOGP, lRes));
            }
        }
    }
    else
    {
        key_value = is_checked;
        lRes = RegSetValueEx(hSectionKey, key_prefix, 0, REG_DWORD,
                             (LPBYTE)(&key_value), sizeof(key_value));
        if (lRes != ERROR_SUCCESS)
        {
            LOGLN0((fi, LOG_ERROR, LOGS "RegSetValueEx failed", LOGP));
        }
    }
    RegCloseKey(hSectionKey);
    return 0;
}

/*****************************************************************************/
static int
finder_save_to_reg(struct finder_info* fi, struct gui_object* go)
{
    HKEY hKey;
    LONG lRes;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    lRes = RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Finder", 0, NULL, 0,
                          KEY_READ | KEY_WRITE, NULL, &hKey, NULL);
    if (lRes != ERROR_SUCCESS)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "RegCreateKeyEx failed", LOGP));
        return 1;
    }
    /* Name/Location tab */
    finder_save_combo(fi, hKey, go->hwndNamedEdit, "NameLocation", "Named");
    finder_save_combo(fi, hKey, go->hwndLookInEdit, "NameLocation", "LookIn");
    finder_save_checkbox(fi, hKey, go->hwndSubfolderCB, "NameLocation", "IncludeSubfolders", TRUE);
    finder_save_checkbox(fi, hKey, go->hwndCaseSensativeCB, "NameLocation", "CaseSensitiveSearch", FALSE);
    finder_save_checkbox(fi, hKey, go->hwndHiddenCB, "NameLocation", "ShowHiddenFiles", FALSE);
    /* Advanced tab */
    finder_save_checkbox(fi, hKey, go->hwndSearchInFileCB, "Advanced", "SearchInFiles", FALSE);
    finder_save_combo(fi, hKey, go->hwndSearchInFileEdit, "Advanced", "SearchInFilesText");
    finder_save_checkbox(fi, hKey, go->hwndCaseSensativeSearchCB, "Advanced", "CaseSensitiveSearch", FALSE);
    RegCloseKey(hKey);
    return 0;
}

/*****************************************************************************/
static int
finder_load_combo(struct finder_info* fi, HKEY hKey, HWND hwnd,
                  const char* section, const char* key_prefix)
{
    int index;
    LONG lRes;
    DWORD key_bytes;
    DWORD type;
    HKEY hSectionKey;
    char key_name[256];
    char key_value[256];

    lRes = RegOpenKeyEx(hKey, section, 0, KEY_READ, &hSectionKey);
    if (lRes != ERROR_SUCCESS)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "RegOpenKeyEx [%s] failed", LOGP, section));
        return 1;
    }
    (void)ComboBox_ResetContent(hwnd);
    for (index = 0; index < 100; index++)
    {
        finder_snprintf(key_name, 255, "%s%2.2d", key_prefix, index);
        key_bytes = 255;
        lRes = RegQueryValueEx(hSectionKey, key_name, NULL, &type, key_value,
                               &key_bytes);
        if (lRes == ERROR_SUCCESS)
        {
            if (type == REG_SZ)
            {
                LOGLN10((fi, LOG_INFO, LOGS "section [%s] key name [%s] key value [%s]", LOGP, section, key_name, key_value));
                (void)ComboBox_AddString(hwnd, key_value);
            }
        }
    }
    if (ComboBox_GetCount(hwnd) > 0)
    {
        (void)ComboBox_SetCurSel(hwnd, 0);
    }
    RegCloseKey(hSectionKey);
    return 0;
}

/*****************************************************************************/
static int
finder_load_checkbox(struct finder_info* fi, HKEY hKey, HWND hwnd,
                     const char* section, const char* key_prefix, BOOL def)
{
    LONG lRes;
    DWORD key_bytes;
    DWORD type;
    HKEY hSectionKey;
    DWORD key_value;
    WPARAM checked;

    lRes = RegOpenKeyEx(hKey, section, 0, KEY_READ, &hSectionKey);
    if (lRes != ERROR_SUCCESS)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "RegOpenKeyEx [%s] failed", LOGP, section));
        /* still need to set default */
        checked = def ? BST_CHECKED : BST_UNCHECKED;
        Button_SetCheck(hwnd, checked);
        return 1;
    }
    key_bytes = sizeof(key_value);
    lRes = RegQueryValueEx(hSectionKey, key_prefix, NULL, &type,
                           (LPBYTE)(&key_value), &key_bytes);
    if (lRes == ERROR_SUCCESS)
    {
        if (type == REG_DWORD)
        {
            LOGLN10((fi, LOG_INFO, LOGS "section [%s] key name [%s] key value [%d]", LOGP, section, key_prefix, key_value));
            def = key_value;
        }
    }
    checked = def ? BST_CHECKED : BST_UNCHECKED;
    Button_SetCheck(hwnd, checked);
    RegCloseKey(hSectionKey);
    return 0;
}

/*****************************************************************************/
static int
finder_load_from_reg(struct finder_info* fi, struct gui_object* go)
{
    HKEY hKey;
    LONG lRes;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    lRes = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Finder", 0,
                        KEY_READ, &hKey);
    if (lRes != ERROR_SUCCESS)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "RegOpenKeyEx failed", LOGP));
        /* still need to set defaults */
        Button_SetCheck(go->hwndSubfolderCB, BST_CHECKED);
        Button_SetCheck(go->hwndCaseSensativeCB, BST_UNCHECKED);
        Button_SetCheck(go->hwndHiddenCB, BST_UNCHECKED);
        Button_SetCheck(go->hwndSearchInFileCB, BST_UNCHECKED);
        Button_SetCheck(go->hwndCaseSensativeSearchCB, BST_UNCHECKED);
        return 1;
    }
    /* Name/Location tab */
    finder_load_combo(fi, hKey, go->hwndNamedEdit, "NameLocation", "Named");
    finder_load_combo(fi, hKey, go->hwndLookInEdit, "NameLocation", "LookIn");
    finder_load_checkbox(fi, hKey, go->hwndSubfolderCB, "NameLocation", "IncludeSubfolders", TRUE);
    finder_load_checkbox(fi, hKey, go->hwndCaseSensativeCB, "NameLocation", "CaseSensitiveSearch", FALSE);
    finder_load_checkbox(fi, hKey, go->hwndHiddenCB, "NameLocation", "ShowHiddenFiles", FALSE);
    /* Advanced tab */
    finder_load_checkbox(fi, hKey, go->hwndSearchInFileCB, "Advanced", "SearchInFiles", FALSE);
    finder_load_combo(fi, hKey, go->hwndSearchInFileEdit, "Advanced", "SearchInFilesText");
    finder_load_checkbox(fi, hKey, go->hwndCaseSensativeSearchCB, "Advanced", "CaseSensitiveSearch", FALSE);
    RegCloseKey(hKey);
    return 0;
}

/*****************************************************************************/
static LRESULT WINAPI
hwndTabs0WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;

    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        /* bad */
        return 0;
    }
    if (uMsg == WM_COMMAND)
    {
        LOGLN10((fi, LOG_INFO, LOGS "got WM_COMMAND wParam 0x%4.4x", LOGP, wParam));
        if (wParam == 0x8804)
        {
            /* forward to main window */
            SendMessage(go->hwnd, uMsg, wParam, lParam);
        }
    }
    return CallWindowProc(go->hwndTabs0WndProcOrg, hwnd, uMsg, wParam, lParam);
}

/*****************************************************************************/
static BOOL
finder_show_window(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;
    DWORD flags;
    LV_COLUMN col;
    TCITEM tie;
    int index;

    (void)wParam;
    (void)lParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return TRUE;
    }
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    if (hwnd != go->hwnd)
    {
        return TRUE;
    }
    /* check if already done */
    if (go->hwndListView != NULL)
    {
        return TRUE;
    }
    /* menu */
    go->hMenubar = CreateMenu();
    go->hMenuFile = CreateMenu();
    AppendMenu(go->hMenuFile, MF_STRING, 0x8801, "&Quit");
    AppendMenu(go->hMenubar, MF_POPUP, (UINT_PTR) go->hMenuFile, "&File");
    go->hMenuHelp = CreateMenu();
    AppendMenu(go->hMenuHelp, MF_STRING, 0x8806, "&Help...");
    AppendMenu(go->hMenuHelp, MF_STRING, 0x8805, "&About");
    AppendMenu(go->hMenubar, MF_POPUP, (UINT_PTR) go->hMenuHelp, "&Help");
    SetMenu(hwnd, go->hMenubar);
    /* status bar */
    flags = WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP;
    go->hwndStatusBar = CreateWindow(STATUSCLASSNAME, "", flags,
                                     0, 0, 0, 0,
                                     hwnd, NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndStatusBar, go->font, FALSE);
    /* create tab control */
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    go->hwndTabControl = CreateWindowEx(WS_EX_CONTROLPARENT,
                                        WC_TABCONTROL, "", flags,
                                        0, 0, 10, 10,
                                        hwnd, NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndTabControl, go->font, FALSE);
    memset(&tie, 0, sizeof(tie));
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1;
    tie.pszText = "Name/Location";
    TabCtrl_InsertItem(go->hwndTabControl, 0, &tie);
    tie.pszText = "Date Modified";
    TabCtrl_InsertItem(go->hwndTabControl, 1, &tie);
    tie.pszText = "Advanced";
    TabCtrl_InsertItem(go->hwndTabControl, 2, &tie);
    /* windows in each tab */
    flags = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_GROUP;
    for (index = 0; index < TAB_NUM_TABS; index++)
    {
        go->hwndTabs[index] = CreateWindowEx(WS_EX_CONTROLPARENT,
                                             "STATIC", "", flags, 0, 0, 10, 10,
                                             go->hwndTabControl,
                                             NULL, go->hInstance, NULL);
    }
    /* so we can catch WM_COMMAND from buttons on tab0 */
    SET_FI_TO_WND(go->hwndTabs[0], fi);
    go->hwndTabs0WndProcOrg = (WNDPROC)
        SetWindowLongPtr(go->hwndTabs[0], GWLP_WNDPROC, (LPARAM)hwndTabs0WndProc);
    /* create list view */
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            LVS_REPORT | LVS_SHOWSELALWAYS;
    go->hwndListView = CreateWindow(WC_LISTVIEW, "", flags, 0, 0, 10, 10,
                                    hwnd, NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndListView, go->font, FALSE);
    (void)ListView_SetExtendedListViewStyle(go->hwndListView, LVS_EX_FULLROWSELECT);
    memset(&col, 0, sizeof(col));
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    col.cx = 100;
    col.pszText = "Name";
    ListView_InsertColumn(go->hwndListView, 0, &col);
    col.pszText = "In Subfolder";
    ListView_InsertColumn(go->hwndListView, 1, &col);
    col.pszText = "Size";
    ListView_InsertColumn(go->hwndListView, 2, &col);
    col.pszText = "Modified";
    ListView_InsertColumn(go->hwndListView, 3, &col);
    /* create buttons */
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    go->hwndFindButton = CreateWindow("BUTTON", "Find", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8802, go->hInstance, NULL);
    SetWindowFont(go->hwndFindButton, go->font, FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_DISABLED;
    go->hwndStopButton = CreateWindow("BUTTON", "Stop", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8803, go->hInstance, NULL);
    SetWindowFont(go->hwndStopButton, go->font, FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_DEFPUSHBUTTON;
    go->hwndExitButton = CreateWindow("BUTTON", "Exit", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8801, go->hInstance, NULL);
    SetWindowFont(go->hwndExitButton, go->font, FALSE);
    /* create tab1 items */
    flags = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
    go->hwndNamedLabel = CreateWindow("STATIC", "Named:", flags,
                                      0, 0, 10, 10,
                                      go->hwndTabs[0],
                                      NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndNamedLabel, go->font, FALSE);
    go->hwndLookInLabel = CreateWindow("STATIC", "Look in:", flags,
                                       0, 0, 10, 10,
                                       go->hwndTabs[0],
                                       NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndLookInLabel, go->font, FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED;
    go->hwndNamedEdit = CreateWindow("COMBOBOX", "", flags, 0, 0, 10, 10,
                                     go->hwndTabs[0],
                                     NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndNamedEdit, go->font, FALSE);
    go->hwndLookInEdit = CreateWindow("COMBOBOX", "", flags, 0, 0, 10, 10,
                                      go->hwndTabs[0],
                                      NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndLookInEdit, go->font, FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    go->hwndBrowseButton = CreateWindow("BUTTON", "Browse", flags, 0, 0, 10, 10,
                                        go->hwndTabs[0], (HMENU)0x8804,
                                        go->hInstance, NULL);
    SetWindowFont(go->hwndBrowseButton, go->font, FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_AUTOCHECKBOX;
    go->hwndSubfolderCB = CreateWindow("BUTTON", "Incude subfolder", flags,
                                       0, 0, 10, 10,
                                       go->hwndTabs[0],
                                       NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndSubfolderCB, go->font, FALSE);
    go->hwndCaseSensativeCB = CreateWindow("BUTTON", "Case sensative search", flags,
                                           0, 0, 10, 10,
                                           go->hwndTabs[0],
                                           NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndCaseSensativeCB, go->font, FALSE);
    go->hwndHiddenCB = CreateWindow("BUTTON", "Show hidden files", flags,
                                    0, 0, 10, 10,
                                    go->hwndTabs[0],
                                    NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndHiddenCB, go->font, FALSE);
    /* create tab3 items */
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_AUTOCHECKBOX;
    go->hwndSearchInFileCB = CreateWindow("BUTTON", "Search in files", flags,
                                          0, 0, 10, 10,
                                          go->hwndTabs[2],
                                          NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndSearchInFileCB, go->font, FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED;
    go->hwndSearchInFileEdit = CreateWindow("COMBOBOX", "", flags,
                                            0, 0, 10, 10,
                                            go->hwndTabs[2],
                                            NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndSearchInFileEdit, go->font, FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_AUTOCHECKBOX;
    go->hwndCaseSensativeSearchCB = CreateWindow("BUTTON",
                                                 "Case sensative search",
                                                 flags, 0, 0, 10, 10,
                                                 go->hwndTabs[2],
                                                 NULL, go->hInstance, NULL);
    SetWindowFont(go->hwndCaseSensativeSearchCB, go->font, FALSE);

    /* the timer will load from registry */
    go->startup_timer = 0x8000;
    SetTimer(go->hwnd, go->startup_timer, 10, NULL);
    finder_load_from_reg(fi, go);

    return TRUE;
}

/*****************************************************************************/
/* resize a combobox causes trouble, this function tries to correct */
/* https://microsoft.public.win32.programmer.ui.narkive.com/68jcCh28/resizing-combo-boxes */
static int
finder_resize_combobox(HWND hwnd, int x, int y, int width, int height)
{
    DWORD combobox_sel;
    char text[256];

    if (!GetWindowText(hwnd, text, 255))
    {
        text[0] = 0;
    }
    combobox_sel = ComboBox_GetEditSel(hwnd);
    MoveWindow(hwnd, x, y, width, height, TRUE);
    SetWindowText(hwnd, text);
    (void)ComboBox_SetEditSel(hwnd, LOWORD(combobox_sel), HIWORD(combobox_sel));
    return 0;
}

/*****************************************************************************/
static BOOL
finder_size(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;
    int width;
    int height;
    int index;
    int x;
    int y;
    int cx;
    int cy;
    RECT rect;
    int iStatusWidths[2];

    (void)wParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return TRUE;
    }
    width = LOWORD(lParam);
    height = HIWORD(lParam);
    if (hwnd == go->hwnd)
    {
        MoveWindow(go->hwndTabControl, 10, 10, width - 120, 200, TRUE);
        GetClientRect(go->hwndTabControl, &rect);
        TabCtrl_AdjustRect(go->hwndTabControl, FALSE, &rect);
        x = rect.left;
        y = rect.top;
        cx = rect.right - rect.left;
        cy = rect.bottom - rect.top;
        for (index = 0; index < TAB_NUM_TABS; index++)
        {
            MoveWindow(go->hwndTabs[index], x, y, cx, cy, TRUE);
        }

        MoveWindow(go->hwndFindButton, width - 95, 30, 75, 25, TRUE);
        MoveWindow(go->hwndStopButton, width - 95, 60, 75, 25, TRUE);
        MoveWindow(go->hwndExitButton, width - 95, 90, 75, 25, TRUE);

        MoveWindow(go->hwndListView, 10, 220, width - 20, height - 240, TRUE);

        MoveWindow(go->hwndNamedLabel, 0, 0, 50, 25, TRUE);
        finder_resize_combobox(go->hwndNamedEdit, 50, 0, width - 190, 150);
        MoveWindow(go->hwndLookInLabel, 0, 30, 50, 25, TRUE);
        finder_resize_combobox(go->hwndLookInEdit, 50, 30, width - 260, 150);
        MoveWindow(go->hwndBrowseButton, width - 200, 30, 50, 25, TRUE);
        MoveWindow(go->hwndSubfolderCB, 0, 60, 150, 25, TRUE);
        MoveWindow(go->hwndCaseSensativeCB, 0, 90, 150, 25, TRUE);
        MoveWindow(go->hwndHiddenCB, 150, 60, 150, 25, TRUE);

        MoveWindow(go->hwndSearchInFileCB, 0, 0, 150, 25, TRUE);
        finder_resize_combobox(go->hwndSearchInFileEdit, 0, 30, width - 190, 150);
        MoveWindow(go->hwndCaseSensativeSearchCB, 0, 60, 150, 25, TRUE);

        SendMessage(go->hwndStatusBar, WM_SIZE, 0, 0);
        iStatusWidths[0] = width - 200;
        iStatusWidths[1] = -1;
        SendMessage(go->hwndStatusBar, SB_SETPARTS, 2, (LPARAM)iStatusWidths);

    }
    return TRUE;
}

/*****************************************************************************/
static INT CALLBACK
BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
    (void)lp;
    if (uMsg == BFFM_INITIALIZED)
    {
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, pData);
    }
    return 0;
}

/*****************************************************************************/
static int
finder_command_browse(struct finder_info* fi, struct gui_object* go)
{
    BROWSEINFO bi;
    char pszBuffer[MAX_PATH];
    char text[MAX_PATH];
    LPITEMIDLIST pidl;
    LPMALLOC lpMalloc;

    /* Initialize COM */
    if (CoInitializeEx(0, COINIT_APARTMENTTHREADED) != S_OK)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "CoInitializeEx failed", LOGP));
        return 0;
    }
    /*  Get a pointer to the shell memory allocator */
    if (SHGetMalloc(&lpMalloc) != S_OK)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "SHGetMalloc failed", LOGP));
        CoUninitialize();
        return 0;
    }
    memset(&bi, 0, sizeof(bi));
    bi.hwndOwner = go->hwnd;
    bi.pszDisplayName = pszBuffer;
    bi.lpszTitle = "Select a Directory";
    bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
    if (GetWindowText(go->hwndLookInEdit, text, 255))
    {
        if (text[0] != 0)
        {
            bi.lpfn = BrowseCallbackProc;
            bi.lParam = (LPARAM)text;
        }
    }
    pidl = SHBrowseForFolder(&bi);
    if (pidl != NULL)
    {
        /* Copy the path directory to the buffer */
        if (SHGetPathFromIDList(pidl, pszBuffer))
        {
            /* pszBuffer now holds the directory path */
            LOGLN0((fi, LOG_INFO, LOGS "Path from dialog [%s]", LOGP, pszBuffer));
            (void)ComboBox_InsertString(go->hwndLookInEdit, 0, pszBuffer);
            (void)ComboBox_SetCurSel(go->hwndLookInEdit, 0);
        }
        IMalloc_Free(lpMalloc, pidl);
    }
    IMalloc_Release(lpMalloc);
    CoUninitialize();
    return 0;
}

#define MAX_TEXT1 1024
#define MAX_TEXT2 1024
#define MAX_TEXT3 (1024 * 1024)

/*****************************************************************************/
static int
finder_listview_to_clipbaord(struct finder_info* fi, struct gui_object* go,
                             BOOL is_full_path)
{
    int index;
    int text2_cur_len;
    int text3_cur_len;
    LVITEM lvi;
    char* text1;
    char* text2;
    char* text3;
    struct lv_item* lvi1;
    HGLOBAL hMem;
    LPVOID locked_mem;
    BOOL do_free;

    text1 = (char*)malloc(MAX_TEXT1 + MAX_TEXT2 + MAX_TEXT3);
    if (text1 == NULL)
    {
        LOGLN0((fi, LOG_ERROR, LOGS "malloc failed", LOGP));
        return 1;
    }
    text2 = text1 + MAX_TEXT1;
    text3 = text2 + MAX_TEXT2;
    text3_cur_len = 0;
    if (is_full_path)
    {
        if (!GetWindowText(go->hwndLookInEdit, text1, MAX_TEXT1))
        {
            text1[0] = 0;
        }
    }
    index = -1;
    while (1)
    {
        index = ListView_GetNextItem(go->hwndListView, index, LVNI_SELECTED);
        if (index == -1)
        {
            break;
        }
        memset(&lvi, 0, sizeof(lvi));
        lvi.mask = LVIF_PARAM;
        lvi.iItem = index;
        LOGLN10((fi, LOG_INFO, LOGS "  index %d", LOGP, index));
        if (ListView_GetItem(go->hwndListView, &lvi))
        {
            text2[0] = 0;
            lvi1 = (struct lv_item*)(lvi.lParam);
            if (lvi1 != NULL)
            {
                if (is_full_path)
                {
                    if ((lvi1->in_subfolder != NULL) && (lvi1->in_subfolder[0] != 0))
                    {
                        finder_snprintf(text2, MAX_TEXT2, "%s\\%s\\%s", text1, lvi1->in_subfolder, lvi1->filename);
                    }
                    else
                    {
                        finder_snprintf(text2, MAX_TEXT2, "%s\\%s", text1, lvi1->filename);
                    }
                }
                else
                {
                    finder_snprintf(text2, MAX_TEXT2, "%s", lvi1->filename);
                }
            }
            text2_cur_len = FINDER_STRLEN(text2);
            if (text3_cur_len + text2_cur_len + 3 < MAX_TEXT3)
            {
                if (text3_cur_len > 0)
                {
                    text3[text3_cur_len++] = '\r';
                    text3[text3_cur_len++] = '\n';
                }
                memcpy(text3 + text3_cur_len, text2, text2_cur_len);
                text3_cur_len += text2_cur_len;
            }
            else
            {
                LOGLN0((fi, LOG_ERROR, LOGS "text too long text3_cur_len %d text2_cur_len %d", LOGP, text3_cur_len, text2_cur_len));
            }
        }
        else
        {
            LOGLN0((fi, LOG_ERROR, LOGS "ListView_GetItem failed index %d", LOGP, index));
        }
    }
    text3[text3_cur_len] = 0;
    index = text3_cur_len + 1;
    LOGLN10((fi, LOG_ERROR, LOGS "text3_cur_len + 1 = %d", LOGP, index));
    hMem =  GlobalAlloc(GMEM_MOVEABLE, index);
    if (hMem != NULL)
    {
        do_free = TRUE;
        locked_mem = GlobalLock(hMem);
        if (locked_mem != NULL)
        {
            memcpy(locked_mem, text3, index);
            GlobalUnlock(hMem);
            if (OpenClipboard(go->hwnd))
            {
                EmptyClipboard();
                if (SetClipboardData(CF_TEXT, hMem) == NULL)
                {
                    LOGLN0((fi, LOG_ERROR, LOGS "SetClipboardData failed", LOGP));
                }
                else
                {
                    /* SetClipboardData success and system owns hMem now */
                    do_free = FALSE;
                }
                CloseClipboard();
            }
            else
            {
                LOGLN0((fi, LOG_ERROR, LOGS "OpenClipboard failed", LOGP));
            }
        }
        else
        {
            LOGLN0((fi, LOG_ERROR, LOGS "GlobalLock failed", LOGP));
        }
        if (do_free)
        {
            GlobalFree(hMem);
        }
    }
    else
    {
        LOGLN0((fi, LOG_ERROR, LOGS "GlobalAlloc failed", LOGP));
    }
    free(text1);
    return 0;
}

/*****************************************************************************/
static BOOL
finder_command(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;

    (void)lParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return TRUE;
    }
    if (hwnd != go->hwnd)
    {
        LOGLN0((fi, LOG_INFO, LOGS "unknown hwnd %p %p", LOGP, hwnd, go->hwnd));
        return TRUE;
    }
    switch (wParam)
    {
        case 0x8801: /* exit */
            LOGLN0((fi, LOG_INFO, LOGS "exit", LOGP));
            PostMessage(go->hwnd, WM_CLOSE, 0, 0);
            break;
        case 0x8802: /* find */
            EnableWindow(go->hwndFindButton, FALSE);
            EnableWindow(go->hwndStopButton, TRUE);
            ListView_DeleteAllItems(go->hwndListView);
            LOGLN0((fi, LOG_INFO, LOGS "starting search", LOGP));
            GetWindowText(go->hwndNamedEdit, fi->named, 255);
            GetWindowText(go->hwndLookInEdit, fi->look_in, 255);
            fi->include_subfolders = Button_GetCheck(go->hwndSubfolderCB);
            fi->case_sensitive = Button_GetCheck(go->hwndCaseSensativeCB);
            fi->show_hidden = Button_GetCheck(go->hwndHiddenCB);
            fi->search_in_files = Button_GetCheck(go->hwndSearchInFileCB);
            GetWindowText(go->hwndSearchInFileEdit, fi->text, 255);
            fi->search_in_case_sensitive = Button_GetCheck(go->hwndCaseSensativeSearchCB);
            /* save data here */
            finder_save_to_reg(fi, go);
            start_find(fi);
            break;
        case 0x8803: /* stop */
            LOGLN0((fi, LOG_INFO, LOGS "stopping search", LOGP));
            stop_find(fi);
            break;
        case 0x8804: /* browse */
            LOGLN0((fi, LOG_INFO, LOGS "browser button", LOGP));
            finder_command_browse(fi, go);
            break;
        case 0x8805: /* about */
            LOGLN0((fi, LOG_INFO, LOGS "about", LOGP));
            break;
        case 0x8806: /* help */
            LOGLN0((fi, LOG_INFO, LOGS "help", LOGP));
            break;
        case 0x8810: /* copy file name */
            LOGLN0((fi, LOG_INFO, LOGS "copy file name", LOGP));
            finder_listview_to_clipbaord(fi, go, FALSE);
            break;
        case 0x8811: /* copy full path */
            LOGLN0((fi, LOG_INFO, LOGS "copy full path", LOGP));
            finder_listview_to_clipbaord(fi, go, TRUE);
            break;
        default:
            LOGLN0((fi, LOG_INFO, LOGS "unknown command 0x%4.4x", LOGP, wParam));
            break;
    }
    return TRUE;
}

/*****************************************************************************/
static int CALLBACK
sort00(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    struct lv_item* obj1;
    struct lv_item* obj2;

    (void)lParamSort;
    obj1 = (struct lv_item*)lParam1;
    obj2 = (struct lv_item*)lParam2;
    return finder_stricmp(obj1->filename, obj2->filename);
}

/*****************************************************************************/
static int CALLBACK
sort01(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    struct lv_item* obj1;
    struct lv_item* obj2;

    (void)lParamSort;
    obj1 = (struct lv_item*)lParam1;
    obj2 = (struct lv_item*)lParam2;
    return finder_stricmp(obj1->in_subfolder, obj2->in_subfolder);
}

/*****************************************************************************/
static int CALLBACK
sort02(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    struct lv_item* obj1;
    struct lv_item* obj2;

    (void)lParamSort;
    obj1 = (struct lv_item*)lParam1;
    obj2 = (struct lv_item*)lParam2;
    if (obj1->size < obj2->size)
    {
        return -1;
    }
    if (obj1->size > obj2->size)
    {
        return 1;
    }
    return 0;
}

/*****************************************************************************/
static int CALLBACK
sort03(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    struct lv_item* obj1;
    struct lv_item* obj2;

    (void)lParamSort;
    obj1 = (struct lv_item*)lParam1;
    obj2 = (struct lv_item*)lParam2;
    return finder_stricmp(obj1->modified, obj2->modified);
}

/*****************************************************************************/
static int CALLBACK
sort10(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    struct lv_item* obj1;
    struct lv_item* obj2;

    (void)lParamSort;
    obj1 = (struct lv_item*)lParam1;
    obj2 = (struct lv_item*)lParam2;
    return finder_stricmp(obj2->filename, obj1->filename);
}

/*****************************************************************************/
static int CALLBACK
sort11(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    struct lv_item* obj1;
    struct lv_item* obj2;

    (void)lParamSort;
    obj1 = (struct lv_item*)lParam1;
    obj2 = (struct lv_item*)lParam2;
    return finder_stricmp(obj2->in_subfolder, obj1->in_subfolder);
}

/*****************************************************************************/
static int CALLBACK
sort12(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    struct lv_item* obj1;
    struct lv_item* obj2;

    (void)lParamSort;
    obj1 = (struct lv_item*)lParam1;
    obj2 = (struct lv_item*)lParam2;
    if (obj2->size < obj1->size)
    {
        return -1;
    }
    if (obj2->size > obj1->size)
    {
        return 1;
    }
    return 0;
}

/*****************************************************************************/
static int CALLBACK
sort13(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    struct lv_item* obj1;
    struct lv_item* obj2;

    (void)lParamSort;
    obj1 = (struct lv_item*)lParam1;
    obj2 = (struct lv_item*)lParam2;
    return finder_stricmp(obj2->modified, obj1->modified);
}

static const PFNLVCOMPARE g_sasc[4] = { sort00, sort01, sort02, sort03 };
static const PFNLVCOMPARE g_sdesc[4] = { sort10, sort11, sort12, sort13 };

/*****************************************************************************/
static BOOL
finder_notify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;
    NMHDR* nm;
    NMLISTVIEW* nmlv;
    struct lv_item* lvi;
    int iSubItem;
    int index;
    HMENU hPopupMenu;
    POINT pt;

    (void)wParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return TRUE;
    }
    if (hwnd != go->hwnd)
    {
        return TRUE;
    }
    nm = (NMHDR*)lParam;
    if (nm->hwndFrom == go->hwndListView)
    {
        switch (nm->code)
        {
            case LVN_COLUMNCLICK:
                nmlv = (NMLISTVIEW*)lParam;
                iSubItem = nmlv->iSubItem % 4;
                if ((go->sort_order[iSubItem] % 2) == 0)
                {
                    ListView_SortItems(go->hwndListView, g_sasc[iSubItem], 0);
                }
                else
                {
                    ListView_SortItems(go->hwndListView, g_sdesc[iSubItem], 0);
                }
                go->sort_order[iSubItem]++;
                break;
            case LVN_DELETEITEM:
                nmlv = (NMLISTVIEW*)lParam;
                lvi = (struct lv_item*)(nmlv->lParam);
                if (lvi != NULL)
                {
                    free(lvi->filename);
                    free(lvi->in_subfolder);
                    free(lvi->size_text);
                    free(lvi->modified);
                    free(lvi);
                }
                break;
            case NM_RCLICK:
                index = ListView_GetNextItem(go->hwndListView, -1, LVNI_SELECTED);
                LOGLN0((fi, LOG_INFO, LOGS "got NM_RCLICK index %d", LOGP, index));
                if (index >= 0)
                {
                    if (GetCursorPos(&pt))
                    {
                        hPopupMenu = CreatePopupMenu();
                        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, 0x8811, "Copy full path");
                        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, 0x8810, "Copy file name");
                        SetForegroundWindow(go->hwnd);
                        TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, go->hwnd, NULL);
                    }
                }
                break;
            case LVN_BEGINDRAG:
                LOGLN0((fi, LOG_INFO, LOGS "LVN_BEGINDRAG", LOGP));
                break;
        }
    }
    else if (nm->hwndFrom == go->hwndTabControl)
    {
        switch (nm->code)
        {
            case TCN_SELCHANGING:
                break;
            case TCN_SELCHANGE:
                index = TabCtrl_GetCurSel(go->hwndTabControl);
                BringWindowToTop(go->hwndTabs[index % TAB_NUM_TABS]);
                break;
        }
    }
    return TRUE;
}

/*****************************************************************************/
static BOOL
finder_close(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;

    (void)wParam;
    (void)lParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return TRUE;
    }
    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    if (MessageBox(hwnd, "Do You want to Exit?", "Finder", MB_YESNO) == IDYES)
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************/
static BOOL
finder_timer(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;

    (void)lParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return TRUE;
    }
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    if (wParam == go->startup_timer)
    {
        KillTimer(hwnd, wParam);
        SendMessage(go->hwndStatusBar, SB_SETTEXT, 0, (LPARAM)"Ready");
        SendMessage(go->hwndStatusBar, SB_SETTEXT, 1, (LPARAM)"");
    }
    return TRUE;
}

/*****************************************************************************/
static BOOL
finder_create(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    (void)hwnd;
    (void)wParam;
    (void)lParam;
    return TRUE;
}

/*****************************************************************************/
static LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL do_def;

    do_def = TRUE;
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SHOWWINDOW:
            do_def = finder_show_window(hwnd, wParam, lParam);
            break;
        case WM_SIZE:
            do_def = finder_size(hwnd, wParam, lParam);
            break;
        case WM_COMMAND:
            do_def = finder_command(hwnd, wParam, lParam);
            break;
        case WM_NOTIFY:
            do_def = finder_notify(hwnd, wParam, lParam);
            break;
        case WM_CLOSE:
            do_def = finder_close(hwnd, wParam, lParam);
            break;
        case WM_TIMER:
            do_def = finder_timer(hwnd, wParam, lParam);
            break;
        case WM_CREATE:
            do_def = finder_create(hwnd, wParam, lParam);
            break;
    }
    if (do_def)
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
