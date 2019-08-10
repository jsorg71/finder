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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>

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

static const char CLASS_NAME[]  = "Finder Window Class";

static LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define GET_FI_FROM_WND(_hwnd) \
    (struct finder_info*)GetWindowLongPtr(_hwnd, GWL_USERDATA)
#define SET_FI_TO_WND(_hwnd, _fi) \
    SetWindowLongPtr(_hwnd, GWL_USERDATA, (LONG_PTR)_fi);

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

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (struct gui_object*)(fi->gui_obj);
    ListView_SetColumnWidth(go->hwndListView, 0, LVSCW_AUTOSIZE_USEHEADER);
    ListView_SetColumnWidth(go->hwndListView, 1, LVSCW_AUTOSIZE_USEHEADER);
    ListView_SetColumnWidth(go->hwndListView, 2, LVSCW_AUTOSIZE_USEHEADER);
    ListView_SetColumnWidth(go->hwndListView, 3, LVSCW_AUTOSIZE_USEHEADER);
    EnableWindow(go->hwndFindButton, TRUE);
    EnableWindow(go->hwndStopButton, FALSE);
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
            "%Ld modified [%s]", LOGP, filename, in_subfolder, size, modified));

    /* allocate */
    while (1)
    {
        lvi = (struct lv_item*)calloc(1, sizeof(struct lv_item));
        if (lvi != NULL)
        {
            lvi->filename = SAFESTRDUP(filename);
            if (lvi->filename != NULL)
            {
                lvi->in_subfolder = SAFESTRDUP(in_subfolder);
                if (lvi->in_subfolder != NULL)
                {
                    lvi->size_text = (char*)calloc(1, 256);
                    if (lvi->size_text != NULL)
                    {
                        lvi->modified = SAFESTRDUP(modified);
                        if (lvi->modified != NULL)
                        {
                            break;
                        }
                        free(lvi->size_text);
                    }
                    free(lvi->in_subfolder);
                }
                free(lvi->filename);
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
    item.cchTextMax = strlen(item.pszText) + 1;
    item.lParam = (LONG_PTR)lvi;
    item.iItem = SendMessage(go->hwndListView, LVM_GETITEMCOUNT, 0, 0);
    SendMessage(go->hwndListView, LVM_INSERTITEM, 0, (LPARAM)&item);

    item.mask = LVIF_TEXT;
    item.iSubItem = 1;
    item.pszText = lvi->in_subfolder;
    item.cchTextMax = strlen(item.pszText) + 1;
    item.lParam = 0;
    SendMessage(go->hwndListView, LVM_SETITEM, 0, (LPARAM)&item);

    item.iSubItem = 2;
    item.pszText = lvi->size_text;
    item.cchTextMax = strlen(item.pszText) + 1;
    SendMessage(go->hwndListView, LVM_SETITEM, 0, (LPARAM)&item);

    item.iSubItem = 3;
    item.pszText = lvi->modified;
    item.cchTextMax = strlen(item.pszText) + 1;
    SendMessage(go->hwndListView, LVM_SETITEM, 0, (LPARAM)&item);

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
    RECT rcClient;
    INITCOMMONCONTROLSEX icex;
    struct finder_info* fi;
    ATOM atom;
    DWORD lasterror;
    NONCLIENTMETRICS non_client_metrics;

    (void)hPrevInstance;
    (void)lpCmdLine;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    atom = RegisterClass(&wc);
    if (atom == 0)
    {
        return 0;
    }
    /* init common controls */
    memset(&icex, 0, sizeof(icex));
    icex.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES;
    if (!InitCommonControlsEx(&icex))
    {
        lasterror = GetLastError();
        LOGLN0((fi, LOG_INFO, LOGS "InitCommonControlsEx failed lasterror 0x%8.8x", LOGP, lasterror));
    }
    fi = (struct finder_info*)calloc(1, sizeof(struct finder_info));
    if (fi == NULL)
    {
        UnregisterClass(CLASS_NAME, hInstance);
        return 0;
    }
    if (gui_init(fi) != 0)
    {
        free(fi);
        UnregisterClass(CLASS_NAME, hInstance);
        return 0;
    }
    go = (struct gui_object*)calloc(1, sizeof(struct gui_object));
    if (go == NULL)
    {
        free(fi);
        UnregisterClass(CLASS_NAME, hInstance);
        return 0;
    }
    go->event = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (go->event == NULL)
    {
        free(go);
        free(fi);
        UnregisterClass(CLASS_NAME, hInstance);
        return 0;
    }
    go->hInstance = hInstance;
    fi->gui_obj = go;
    go->hwnd = CreateWindow(CLASS_NAME, "Finder", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, NULL, hInstance, NULL);
    if (go->hwnd == NULL)
    {
        CloseHandle(go->event);
        free(go);
        free(fi);
        UnregisterClass(CLASS_NAME, hInstance);
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
    UnregisterClass(CLASS_NAME, hInstance);
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
        index = SendMessage(hwnd, CB_FINDSTRING, 0, (LPARAM)text);
        if (index == CB_ERR)
        {
            SendMessage(hwnd, CB_INSERTSTRING, 0, (LPARAM)text);
        }
        else if (index > 0)
        {
            SendMessage(hwnd, CB_DELETESTRING, index, 0);
            SendMessage(hwnd, CB_INSERTSTRING, 0, (LPARAM)text);
        }
        SendMessage(hwnd, CB_SETCURSEL, 0, 0);
    }
    for (index = 0; index < 100; index++)
    {
        snprintf(key_name, 255, "%s%2.2d", key_prefix, index);
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
    count = SendMessage(hwnd, CB_GETCOUNT, 0, 0);
    LOGLN10((fi, LOG_INFO, LOGS "for edit [%s] got count %d", LOGP, key_prefix, count));
    if (count != CB_ERR)
    {
        if (count > 100)
        {
            count = 100;
        }
        for (index = 0; index < count; index++)
        {
            cb_text_bytes = SendMessage(hwnd, CB_GETLBTEXTLEN, index, 0);
            if (cb_text_bytes != CB_ERR)
            {
                if (cb_text_bytes > 0)
                {
                    cb_text = (char*)malloc(cb_text_bytes + 1);
                    if (cb_text != NULL)
                    {
                        cb_text_bytes1 = SendMessage(hwnd, CB_GETLBTEXT, index, (LPARAM)cb_text);
                        if (cb_text_bytes == cb_text_bytes1)
                        {
                            snprintf(key_name, 255, "%s%2.2d", key_prefix, index);
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

    is_checked = SendMessage(hwnd, BM_GETCHECK, 0, 0);
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
    SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
    for (index = 0; index < 100; index++)
    {
        snprintf(key_name, 255, "%s%2.2d", key_prefix, index);
        key_bytes = 255;
        lRes = RegQueryValueEx(hSectionKey, key_name, NULL, &type, key_value,
                               &key_bytes);
        if (lRes == ERROR_SUCCESS)
        {
            if (type == REG_SZ)
            {
                LOGLN10((fi, LOG_INFO, LOGS "section [%s] key name [%s] key value [%s]", LOGP, section, key_name, key_value));
                SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)key_value);
            }
        }
    }
    if (SendMessage(hwnd, CB_GETCOUNT, 0, 0) > 0)
    {
        SendMessage(hwnd, CB_SETCURSEL, 0, 0);
    }
    RegCloseKey(hSectionKey);
    return 0;
}

/*****************************************************************************/
static int
finder_load_checkbox(struct finder_info* fi, HKEY hKey, HWND hwnd,
                     const char* section, const char* key_prefix, BOOL def)
{
    int index;
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
        SendMessage(hwnd, BM_SETCHECK, checked, 0);
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
    SendMessage(hwnd, BM_SETCHECK, checked, 0);
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
        SendMessage(go->hwndSubfolderCB, BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(go->hwndCaseSensativeCB, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessage(go->hwndHiddenCB, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessage(go->hwndSearchInFileCB, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessage(go->hwndCaseSensativeSearchCB, BM_SETCHECK, BST_UNCHECKED, 0);
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
static int
finder_show_window(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;
    DWORD flags;
    LV_COLUMN col;
    TCITEM tie;
    int index;
    HMENU hMenubar;
    HMENU hMenuFile;
    HMENU hMenuHelp;

    (void)wParam;
    (void)lParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
    }
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    if (hwnd != go->hwnd)
    {
        return 0;
    }
    /* check if already done */
    if (go->hwndListView != NULL)
    {
        return 0;
    }
    /* menu */
    hMenubar = CreateMenu();
    hMenuFile = CreateMenu();
    AppendMenu(hMenuFile, MF_STRING, 0x8801, "&Quit");
    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR) hMenuFile, "&File");
    hMenuHelp = CreateMenu();
    AppendMenu(hMenuHelp, MF_STRING, 0x8805, "&About");
    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR) hMenuHelp, "&Help");
    SetMenu(hwnd, hMenubar);
    /* create tab control */
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    go->hwndTabControl = CreateWindowEx(WS_EX_CONTROLPARENT,
                                        WC_TABCONTROL, "", flags,
                                        0, 0, 10, 10,
                                        hwnd, NULL, go->hInstance, NULL);
    SendMessage(go->hwndTabControl, WM_SETFONT, (WPARAM)(go->font), FALSE);
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
    /* create list view */
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            LVS_REPORT | LVS_SHOWSELALWAYS;
    go->hwndListView = CreateWindow(WC_LISTVIEW, "", flags, 0, 0, 10, 10,
                                    hwnd, NULL, go->hInstance, NULL);
    SendMessage(go->hwndListView, WM_SETFONT, (WPARAM)(go->font), FALSE);
    SendMessage(go->hwndListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
                LVS_EX_FULLROWSELECT);
    memset(&col, 0, sizeof(col));
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    col.cx = 100;
    col.pszText = "Name";
    SendMessage(go->hwndListView, LVM_INSERTCOLUMN, 0, (LPARAM)&col);
    col.pszText = "In Subfolder";
    SendMessage(go->hwndListView, LVM_INSERTCOLUMN, 1, (LPARAM)&col);
    col.pszText = "Size";
    SendMessage(go->hwndListView, LVM_INSERTCOLUMN, 2, (LPARAM)&col);
    col.pszText = "Modified";
    SendMessage(go->hwndListView, LVM_INSERTCOLUMN, 3, (LPARAM)&col);
    /* create buttons */
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    go->hwndFindButton = CreateWindow("BUTTON", "Find", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8802, go->hInstance, NULL);
    SendMessage(go->hwndFindButton, WM_SETFONT, (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_DISABLED;
    go->hwndStopButton = CreateWindow("BUTTON", "Stop", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8803, go->hInstance, NULL);
    SendMessage(go->hwndStopButton, WM_SETFONT, (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_DEFPUSHBUTTON;
    go->hwndExitButton = CreateWindow("BUTTON", "Exit", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8801, go->hInstance, NULL);
    SendMessage(go->hwndExitButton, WM_SETFONT, (WPARAM)(go->font), FALSE);
    /* create tab1 items */
    flags = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
    go->hwndNamedLabel = CreateWindow("STATIC", "Named:", flags,
                                      0, 0, 10, 10,
                                      go->hwndTabs[0],
                                      NULL, go->hInstance, NULL);
    SendMessage(go->hwndNamedLabel, WM_SETFONT, (WPARAM)(go->font), FALSE);
    go->hwndLookInLabel = CreateWindow("STATIC", "Look in:", flags,
                                       0, 0, 10, 10,
                                       go->hwndTabs[0],
                                       NULL, go->hInstance, NULL);
    SendMessage(go->hwndLookInLabel, WM_SETFONT, (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED;
    go->hwndNamedEdit = CreateWindow("COMBOBOX", "", flags, 0, 0, 10, 10,
                                     go->hwndTabs[0],
                                     NULL, go->hInstance, NULL);
    SendMessage(go->hwndNamedEdit, WM_SETFONT, (WPARAM)(go->font), FALSE);
    go->hwndLookInEdit = CreateWindow("COMBOBOX", "", flags, 0, 0, 10, 10,
                                      go->hwndTabs[0],
                                      NULL, go->hInstance, NULL);
    SendMessage(go->hwndLookInEdit, WM_SETFONT, (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    go->hwndBrowseButton = CreateWindow("BUTTON", "Browse", flags, 0, 0, 10, 10,
                                        go->hwndTabs[0], (HMENU)0x8804,
                                        go->hInstance, NULL);
    SendMessage(go->hwndBrowseButton, WM_SETFONT, (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_AUTOCHECKBOX;
    go->hwndSubfolderCB = CreateWindow("BUTTON", "Incude subfolder", flags,
                                       0, 0, 10, 10,
                                       go->hwndTabs[0],
                                       NULL, go->hInstance, NULL);
    SendMessage(go->hwndSubfolderCB, WM_SETFONT, (WPARAM)(go->font), FALSE);
    go->hwndCaseSensativeCB = CreateWindow("BUTTON", "Case sensative search", flags,
                                           0, 0, 10, 10,
                                           go->hwndTabs[0],
                                           NULL, go->hInstance, NULL);
    SendMessage(go->hwndCaseSensativeCB, WM_SETFONT, (WPARAM)(go->font), FALSE);
    go->hwndHiddenCB = CreateWindow("BUTTON", "Show hidden files", flags,
                                    0, 0, 10, 10,
                                    go->hwndTabs[0],
                                    NULL, go->hInstance, NULL);
    SendMessage(go->hwndHiddenCB, WM_SETFONT, (WPARAM)(go->font), FALSE);
    /* create tab3 items */
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_AUTOCHECKBOX;
    go->hwndSearchInFileCB = CreateWindow("BUTTON", "Search in files", flags,
                                          0, 0, 10, 10,
                                          go->hwndTabs[2],
                                          NULL, go->hInstance, NULL);
    SendMessage(go->hwndSearchInFileCB, WM_SETFONT, (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED;
    go->hwndSearchInFileEdit = CreateWindow("COMBOBOX", "", flags,
                                            0, 0, 10, 10,
                                            go->hwndTabs[2],
                                            NULL, go->hInstance, NULL);
    SendMessage(go->hwndSearchInFileEdit, WM_SETFONT,
                (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_AUTOCHECKBOX;
    go->hwndCaseSensativeSearchCB = CreateWindow("BUTTON",
                                                 "Case sensative search",
                                                 flags, 0, 0, 10, 10,
                                                 go->hwndTabs[2],
                                                 NULL, go->hInstance, NULL);
    SendMessage(go->hwndCaseSensativeSearchCB, WM_SETFONT,
                (WPARAM)(go->font), FALSE);

    /* the timer will load from registry */
    go->startup_timer = 0x8000;
    SetTimer(go->hwnd, go->startup_timer, 10, NULL);
    finder_load_from_reg(fi, go);

    return 0;
}

/*****************************************************************************/
/* resize a combobox causes trouble, this function tries to correct */
/* https://microsoft.public.win32.programmer.ui.narkive.com/68jcCh28/resizing-combo-boxes */
static int
finder_resize_combobox(HWND hwnd, int x, int y, int width, int height)
{
    int combobox_sel;
    char text[256];

    GetWindowText(hwnd, text, 255);
    combobox_sel = SendMessage(hwnd, CB_GETEDITSEL, 0, 0);
    MoveWindow(hwnd, x, y, width, height, TRUE);
    SetWindowText(hwnd, text);
    SendMessage(hwnd, CB_SETEDITSEL, 0, combobox_sel);
    return 0;
}

/*****************************************************************************/
static int
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

    (void)wParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
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
    }
    return 0;
}

/*****************************************************************************/
static int
finder_command(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;

    (void)lParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
    }
    if (hwnd != go->hwnd)
    {
        return 0;
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
            SendMessage(go->hwndListView, LVM_DELETEALLITEMS, 0, 0);
            LOGLN0((fi, LOG_INFO, LOGS "starting search", LOGP));
            GetWindowText(go->hwndNamedEdit, fi->named, 255);
            GetWindowText(go->hwndLookInEdit, fi->look_in, 255);
            LOGLN0((fi, LOG_INFO, "look in %s", fi->look_in));
            fi->include_subfolders = SendMessage(go->hwndSubfolderCB, BM_GETCHECK, 0, 0);
            LOGLN0((fi, LOG_INFO, "include subfolders %d", fi->include_subfolders));
            fi->case_sensitive = SendMessage(go->hwndCaseSensativeCB, BM_GETCHECK, 0, 0);
            LOGLN0((fi, LOG_INFO, "case sensative %d", fi->case_sensitive));
            fi->show_hidden = SendMessage(go->hwndHiddenCB, BM_GETCHECK, 0, 0);
            LOGLN0((fi, LOG_INFO, "show_hidden %d", fi->show_hidden));
            fi->search_in_files = SendMessage(go->hwndSearchInFileCB, BM_GETCHECK, 0, 0);
            GetWindowText(go->hwndSearchInFileEdit, fi->text, 255);
            fi->search_in_case_sensitive = SendMessage(go->hwndCaseSensativeSearchCB, BM_GETCHECK, 0, 0);
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
            break;
        case 0x8805: /* about */
            LOGLN0((fi, LOG_INFO, LOGS "about", LOGP));
            break;
    }
    return 0;
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
    return stricmp(obj1->filename, obj2->filename);
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
    return stricmp(obj1->in_subfolder, obj2->in_subfolder);
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
    return stricmp(obj1->modified, obj2->modified);
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
    return stricmp(obj2->filename, obj1->filename);
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
    return stricmp(obj2->in_subfolder, obj1->in_subfolder);
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
    return stricmp(obj2->modified, obj1->modified);
}

static const PFNLVCOMPARE g_sasc[4] = { sort00, sort01, sort02, sort03 };
static const PFNLVCOMPARE g_sdesc[4] = { sort10, sort11, sort12, sort13 };

/*****************************************************************************/
static int
finder_notify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;
    NMHDR* nm;
    NMLISTVIEW* nmlv;
    struct lv_item* lvi;
    int iSubItem;
    int index;

    (void)wParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
    }
    if (hwnd != go->hwnd)
    {
        return 0;
    }
    nm = (NMHDR*)lParam;
    if (nm->hwndFrom == go->hwndListView)
    {
        nmlv = (NMLISTVIEW*)lParam;
        switch (nm->code)
        {
            case LVN_COLUMNCLICK:
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
    return 0;
}

/*****************************************************************************/
static int
finder_close(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;

    (void)wParam;
    (void)lParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
    }
    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    return 0;
}

/*****************************************************************************/
static int
finder_timer(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;

    (void)lParam;
    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
    }
    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    if (wParam == go->startup_timer)
    {
        KillTimer(hwnd, wParam);
        //finder_load_from_reg(fi, go);
    }
    return 0;
}

/*****************************************************************************/
static int
finder_create(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    (void)hwnd;
    (void)wParam;
    (void)lParam;
    return 0;
}

/*****************************************************************************/
static LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SHOWWINDOW:
            finder_show_window(hwnd, wParam, lParam);
            break;
        case WM_SIZE:
            finder_size(hwnd, wParam, lParam);
            break;
        case WM_COMMAND:
            finder_command(hwnd, wParam, lParam);
            break;
        case WM_NOTIFY:
            finder_notify(hwnd, wParam, lParam);
            break;
        case WM_CLOSE:
            finder_close(hwnd, wParam, lParam);
            break;
        case WM_TIMER:
            finder_timer(hwnd, wParam, lParam);
            break;
        case WM_CREATE:
            finder_create(hwnd, wParam, lParam);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
