
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>

#include "finder.h"
#include "finder_event.h"

struct gui_object
{
    HINSTANCE hInstance;
    HWND hwnd;
    HWND hwndExitButton;
    HWND hwndFindButton;
    HWND hwndStopButton;
    HWND hwndListView;
    HWND hwndTabControl;
    HANDLE event;
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
    //struct gui_object* go;

    writeln(fi, "gui_find_done:");
    //go = (struct gui_object*)(fi->gui_obj);
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
    char text[256];

    go = (struct gui_object*)(fi->gui_obj);
    //writeln(fi, "gui_add_one: filename [%s] subfolder [%s] size %Ld "
    //        "modified [%s]", filename, in_subfolder, size, modified);

    memset(&item, 0, sizeof(item));
    item.mask = LVIF_TEXT;
    item.cchTextMax = 256;
    item.iItem = 0;
    item.iSubItem = 0;
    snprintf(text, 255, "%s", filename);
    item.pszText = text;
    SendMessage(go->hwndListView, LVM_INSERTITEM, 0, (LPARAM)&item);

    item.iSubItem = 1;
    snprintf(text, 255, "%s", in_subfolder);
    SendMessage(go->hwndListView, LVM_SETITEM, 0, (LPARAM)&item);

    item.iSubItem = 2;
    format_commas(size, text);
    //snprintf(text, 255, "%Ld", size);
    SendMessage(go->hwndListView, LVM_SETITEM, 0, (LPARAM)&item);

    item.iSubItem = 3;
    snprintf(text, 255, "%s", modified);
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

    (void)hPrevInstance;
    (void)lpCmdLine;

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
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
    ShowWindow(go->hwnd, nCmdShow);

    memset(&icex, 0, sizeof(icex));
    //icex.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES;
    icex.dwICC = ICC_LISTVIEW_CLASSES;
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
            //writeln(fi, "WinMain: got event");
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
    gui_deinit(fi);
    DestroyWindow(go->hwnd);
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

    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
    }
    writeln(fi, "finder_show_window:");
    if (hwnd != go->hwnd)
    {
        return 0;
    }
    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            LVS_REPORT;
    go->hwndListView = CreateWindow(WC_LISTVIEW, "", flags, 0, 0, 10, 10,
                                    hwnd, NULL, go->hInstance, NULL);

    SendMessage(go->hwndListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
                LVS_EX_FULLROWSELECT);

    memset(&col, 0, sizeof(col));
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    col.cx = 100;
    col.pszText = "filename";
    SendMessage(go->hwndListView, LVM_INSERTCOLUMN, 0, (LPARAM)&col);
    col.pszText = "in subfolder";
    SendMessage(go->hwndListView, LVM_INSERTCOLUMN, 1, (LPARAM)&col);
    col.pszText = "size";
    SendMessage(go->hwndListView, LVM_INSERTCOLUMN, 2, (LPARAM)&col);
    col.pszText = "modified";
    SendMessage(go->hwndListView, LVM_INSERTCOLUMN, 3, (LPARAM)&col);

    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    go->hwndTabControl = CreateWindow(WC_TABCONTROL, "", flags, 0, 0, 10, 10,
                                      hwnd, NULL, go->hInstance, NULL);
    memset(&tie, 0, sizeof(tie));
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1;
    tie.pszText = "Name/Location";
    TabCtrl_InsertItem(go->hwndTabControl, 0, &tie);
    tie.pszText = "Date Modified";
    TabCtrl_InsertItem(go->hwndTabControl, 1, &tie);
    tie.pszText = "Advanced";
    TabCtrl_InsertItem(go->hwndTabControl, 2, &tie);

    flags = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
            BS_DEFPUSHBUTTON;
    go->hwndExitButton = CreateWindow("BUTTON", "Exit", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8801, go->hInstance, NULL);
    go->hwndFindButton = CreateWindow("BUTTON", "Find", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8802, go->hInstance, NULL);
    go->hwndStopButton = CreateWindow("BUTTON", "Stop", flags, 0, 0, 10, 10,
                                      hwnd, (HMENU)0x8803, go->hInstance, NULL);

    writeln(fi, "finder_show_window: starting search");
    snprintf(fi->named, 255, "*.*");
    snprintf(fi->look_in, 255, "d:\\flats");
    fi->include_subfolders = 1;
    fi->case_sensitive = 0;
    start_find(fi);

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

    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
    }
    if (hwnd != go->hwnd)
    {
        return 0;
    }
    width = LOWORD(lParam);
    height = HIWORD(lParam);
    //writeln(&g_fi, "finder_size: wParam %d width %d height %d", wParam, width, height);
    MoveWindow(go->hwndExitButton, 0, 0, 100, 100, TRUE);
    MoveWindow(go->hwndFindButton, 100, 0, 100, 100, TRUE);
    MoveWindow(go->hwndStopButton, 200, 0, 100, 100, TRUE);
    MoveWindow(go->hwndListView, 10, height / 2, width - 20, height / 2 - 20, TRUE);
    MoveWindow(go->hwndTabControl, 300, 0, 1000, 100, TRUE);
    return 0;
}

/*****************************************************************************/
static int
finder_command(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;

    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
    }
    if (hwnd != go->hwnd)
    {
        return 0;
    }
    if (wParam == 0x8801) /* exit */
    {
        PostMessage(go->hwnd, WM_CLOSE, 0, 0);
    }
    if (wParam == 0x8802) /* find */
    {
        SendMessage(go->hwndListView, LVM_DELETEALLITEMS, 0, 0);
        writeln(fi, "finder_show_window: starting search");
        snprintf(fi->named, 255, "*.cab");
        //snprintf(fi->look_in, 255, "d:\\flats");
        snprintf(fi->look_in, 255, "d:\\windows");
        fi->include_subfolders = 1;
        fi->case_sensitive = 0;
        start_find(fi);
    }
    if (wParam == 0x8803) /* stop */
    {
        writeln(fi, "finder_show_window: stopping search");
        stop_find(fi);
    }
    return 0;
}

/*****************************************************************************/
static int
finder_notify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    struct gui_object* go;
    NMHDR* nm;
    NMLISTVIEW* nmlv;

    if (get_fi_go_from_hwnd(hwnd, &fi, &go) != 0)
    {
        return 0;
    }
    if (hwnd != go->hwnd)
    {
        return 0;
    }
    //writeln(fi, "finder_notify: hwnd %d wParam %d lParam %d", hwnd, wParam, lParam);
    nm = (NMHDR*)lParam;
    if (nm->hwndFrom == go->hwndListView)
    {
        nmlv = (NMLISTVIEW*)lParam;
        switch (nm->code)
        {
            case LVN_COLUMNCLICK:
                //writeln(fi, "finder_notify: got LVN_COLUMNCLICK");
                writeln(fi, "finder_notify: got LVN_COLUMNCLICK column %d", nmlv->iSubItem);
                break;
            case LVN_DELETEITEM:
                writeln(fi, "finder_notify: got LVN_DELETEITEM lparam %p", (void*)(nmlv->lParam));
                break;
        }
    }
    return 0;
}
/*****************************************************************************/
static LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    struct finder_info* fi;
    PAINTSTRUCT ps;
    HDC hdc;

    fi = GET_FI_FROM_WND(hwnd);
    if (fi == NULL)
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            break;
        case WM_SHOWWINDOW:
            writeln(fi, "WindowProc: WM_SHOWWINDOW");
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
