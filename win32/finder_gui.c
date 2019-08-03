
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
    HWND hwndExitButton;
    HWND hwndFindButton;
    HWND hwndStopButton;
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

    //writeln(fi, "gui_set_event:");
    go = (struct gui_object*)(fi->gui_obj);
    SetEvent(go->event);
    return 0;
}

/*****************************************************************************/
int
gui_find_done(struct finder_info* fi)
{
    struct gui_object* go;

    writeln(fi, "gui_find_done:");
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
    //writeln(fi, "gui_add_one: filename [%s] subfolder [%s] size %Ld "
    //        "modified [%s]", filename, in_subfolder, size, modified);

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
    format_commas(size, lvi->size_text);

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
    /* init common controls */
    memset(&icex, 0, sizeof(icex));
    icex.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES;
    if (!InitCommonControlsEx(&icex))
    {
        lasterror = GetLastError();
        writeln(fi, "WinMain: InitCommonControlsEx failed lasterror 0x%8.8x", lasterror);
    }
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
                writeln(fi, "WinMain: got quit");
                cont = 0;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
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
        return 0;
    }
    writeln(fi, "finder_show_window:");
    if (hwnd != go->hwnd)
    {
        return 0;
    }
    /* check if already done */
    if (go->hwndListView != NULL)
    {
        return 0;
    }
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            LVS_REPORT;
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

    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    go->hwndTabControl = CreateWindow(WC_TABCONTROL, "", flags, 0, 0, 10, 10,
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
    
    //flags = WS_CHILD | WS_VISIBLE;
    flags = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
    for (index = 0; index < TAB_NUM_TABS; index++)
    {
        go->hwndTabs[index] = CreateWindow("STATIC", "", flags, 0, 0, 10, 10,
                                           go->hwndTabControl,
                                           NULL, go->hInstance, NULL);
    }

    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_DEFPUSHBUTTON;
    go->hwndExitButton = CreateWindow("BUTTON", "Exit", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8801, go->hInstance, NULL);
    SendMessage(go->hwndExitButton, WM_SETFONT, (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    go->hwndFindButton = CreateWindow("BUTTON", "Find", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8802, go->hInstance, NULL);
    SendMessage(go->hwndFindButton, WM_SETFONT, (WPARAM)(go->font), FALSE);
    go->hwndBrowseButton = CreateWindow("BUTTON", "Browse", flags, 0, 0, 10, 10,
                                        go->hwndTabs[0], (HMENU)0x8804,
                                        go->hInstance, NULL);
    SendMessage(go->hwndBrowseButton, WM_SETFONT, (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_DISABLED;
    go->hwndStopButton = CreateWindow("BUTTON", "Stop", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8803, go->hInstance, NULL);
    SendMessage(go->hwndStopButton, WM_SETFONT, (WPARAM)(go->font), FALSE);

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
    go->hwndNamedEdit = CreateWindow(WC_COMBOBOXEX, "", flags, 0, 0, 10, 10,
                                     go->hwndTabs[0],
                                     NULL, go->hInstance, NULL);
    SendMessage(go->hwndNamedEdit, WM_SETFONT, (WPARAM)(go->font), FALSE);
    go->hwndLookInEdit = CreateWindow(WC_COMBOBOXEX, "", flags, 0, 0, 10, 10,
                                      go->hwndTabs[0],
                                      NULL, go->hInstance, NULL);
    SendMessage(go->hwndLookInEdit, WM_SETFONT, (WPARAM)(go->font), FALSE);

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

    go->hwndSearchInFileCB = CreateWindow("BUTTON", "Search in files", flags,
                                          0, 0, 10, 10,
                                          go->hwndTabs[2],
                                          NULL, go->hInstance, NULL);
    SendMessage(go->hwndSearchInFileCB, WM_SETFONT, (WPARAM)(go->font), FALSE);
    go->hwndCaseSensativeSearchCB = CreateWindow("BUTTON",
                                                 "Case sensative search",
                                                 flags, 0, 0, 10, 10,
                                                 go->hwndTabs[2],
                                                 NULL, go->hInstance, NULL);
    SendMessage(go->hwndCaseSensativeSearchCB, WM_SETFONT,
                (WPARAM)(go->font), FALSE);
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED;
    go->hwndSearchInFileEdit = CreateWindow(WC_COMBOBOXEX, "", flags,
                                            0, 0, 10, 10,
                                            go->hwndTabs[2],
                                            NULL, go->hInstance, NULL);
    SendMessage(go->hwndSearchInFileEdit, WM_SETFONT,
                (WPARAM)(go->font), FALSE);

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

        MoveWindow(go->hwndExitButton, width - 95, 30, 75, 25, TRUE);
        MoveWindow(go->hwndFindButton, width - 95, 60, 75, 25, TRUE);
        MoveWindow(go->hwndStopButton, width - 95, 90, 75, 25, TRUE);

        MoveWindow(go->hwndListView, 10, 220, width - 20, height - 240, TRUE);

        MoveWindow(go->hwndNamedLabel, 0, 0, 50, 25, TRUE);
        MoveWindow(go->hwndNamedEdit, 50, 0, width - 190, 25, TRUE);
        MoveWindow(go->hwndLookInLabel, 0, 30, 50, 25, TRUE);
        MoveWindow(go->hwndLookInEdit, 50, 30, width - 260, 25, TRUE);
        MoveWindow(go->hwndBrowseButton, width - 200, 30, 50, 25, TRUE);
        MoveWindow(go->hwndSubfolderCB, 0, 60, 150, 25, TRUE);
        MoveWindow(go->hwndCaseSensativeCB, 0, 90, 150, 25, TRUE);
        MoveWindow(go->hwndHiddenCB, 150, 60, 150, 25, TRUE);

        MoveWindow(go->hwndSearchInFileCB, 0, 0, 150, 25, TRUE);
        MoveWindow(go->hwndSearchInFileEdit, 0, 30, width - 190, 25, TRUE);
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
            PostMessage(go->hwnd, WM_CLOSE, 0, 0);
            break;
        case 0x8802: /* find */
            EnableWindow(go->hwndFindButton, FALSE);
            EnableWindow(go->hwndStopButton, TRUE);
            SendMessage(go->hwndListView, LVM_DELETEALLITEMS, 0, 0);
            writeln(fi, "finder_show_window: starting search");
            //snprintf(fi->named, 255, "*.cab");
            //snprintf(fi->look_in, 255, "d:\\flats");
            //snprintf(fi->look_in, 255, "d:\\windows");

            GetWindowText(go->hwndNamedEdit, fi->named, 255);

            GetWindowText(go->hwndLookInEdit, fi->look_in, 255);
            writeln(fi, "look in %s", fi->look_in);

            fi->include_subfolders = SendMessage(go->hwndSubfolderCB, BM_GETCHECK, 0, 0);
            writeln(fi, "include subfolders %d", fi->include_subfolders);

            fi->case_sensitive = SendMessage(go->hwndCaseSensativeCB, BM_GETCHECK, 0, 0);
            writeln(fi, "case sensative %d", fi->case_sensitive);

            fi->show_hidden = SendMessage(go->hwndHiddenCB, BM_GETCHECK, 0, 0);
            writeln(fi, "show_hidden %d", fi->show_hidden);

            fi->search_in_files = SendMessage(go->hwndSearchInFileCB, BM_GETCHECK, 0, 0);
            GetWindowText(go->hwndSearchInFileEdit, fi->text, 255);
            fi->search_in_case_sensitive = SendMessage(go->hwndCaseSensativeSearchCB, BM_GETCHECK, 0, 0);

            start_find(fi);
            break;
        case 0x8803: /* stop */
            writeln(fi, "finder_show_window: stopping search");
            stop_find(fi);
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
    return (int)(obj1->size - obj2->size);
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
    return (int)(obj2->size - obj1->size);
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
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
