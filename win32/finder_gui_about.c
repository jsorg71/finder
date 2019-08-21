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
#include "resources.h"

#include "finder.h"

/*****************************************************************************/
static INT_PTR CALLBACK
AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char text[128];

    (void)lParam;
    switch (uMsg)
    {
        case WM_INITDIALOG:
            finder_snprintf(text, 127, "Finder version %d.%d",
                            FINDER_VERSION_MAJOR, FINDER_VERSION_MINOR);
            SetDlgItemText(hDlg, IDD_DIALOG_ABOUT_TEXT, text);
            return TRUE;
        case WM_CLOSE:
            EndDialog(hDlg, IDCANCEL);
            break;
        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                    EndDialog(hDlg, IDOK);
                    break;
            }
            break;
    }
    return FALSE;
}

/*****************************************************************************/
int
finder_gui_about(struct finder_info* fi, HINSTANCE hInstance, HWND owner)
{
    DLGPROC proc;
    LPCTSTR template;
    int rv;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    template = MAKEINTRESOURCE(IDD_DIALOG_ABOUT);
    proc = AboutDlgProc;
    rv = (int)DialogBox(hInstance, template, owner, proc);
    LOGLN0((fi, LOG_INFO, LOGS "done rv %d", LOGP, rv));
    return rv;
}