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

#define COBJMACROS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
//#include <shlobj.h>

#include "finder.h"
#include "finder_gui_dd.h"

struct tagMYIDROPSOURCE
{
    IDropSource ids;
    LONG lRefCount;
    BOOL bRightClick;
    HWND hWndMenu;
    HMENU hPopup;
};
typedef struct tagMYIDROPSOURCE MYIDROPSOURCE;
typedef struct tagMYIDROPSOURCE* PMYIDROPSOURCE;

struct tagMYIDROPSOURCE_VTBL
{
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(PMYIDROPSOURCE pThis, REFIID riid, void** ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(PMYIDROPSOURCE pThis);
    ULONG (STDMETHODCALLTYPE * Release)(PMYIDROPSOURCE pThis);
    HRESULT (STDMETHODCALLTYPE * QueryContinueDrag)(PMYIDROPSOURCE pThis, BOOL fEscapePressed, DWORD dwKeyState);
    HRESULT (STDMETHODCALLTYPE * GiveFeedback)(PMYIDROPSOURCE pThis, DWORD dwEffect);
    END_INTERFACE
};
typedef struct tagMYIDROPSOURCE_VTBL MYIDROPSOURCE_VTBL;
typedef struct tagMYIDROPSOURCE_VTBL* PMYIDROPSOURCE_VTBL;

/*****************************************************************************/
int
finder_gui_dd_init(void)
{
    OleInitialize(NULL);
}
