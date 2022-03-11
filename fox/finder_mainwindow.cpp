/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2020-2022
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
#include <fx.h>

#include "finder.h"
#include "finder_gui.h"
#include "finder_mainwindow.h"

/*****************************************************************************/
FXFinderMainWindow::FXFinderMainWindow()
{
}

/*****************************************************************************/
FXFinderMainWindow::FXFinderMainWindow(FXApp* app):
                    FXMainWindow(app, "Finder", NULL, NULL, DECOR_ALL,
                                 0, 0, 640, 480)
{
}

/*****************************************************************************/
FXFinderMainWindow::~FXFinderMainWindow()
{
}

/*****************************************************************************/
long
FXFinderMainWindow::onConfigure(FXObject* obj, FXSelector sel, void* ptr)
{
    FXMainWindow::onConfigure(obj, sel, ptr);
    if (target != NULL)
    {
        target->tryHandle(this, FXSEL(SEL_FINDERCONF, message), NULL);
    }
    return 1;
}

FXDEFMAP(FXFinderMainWindow) FXFinderMainWindowMap[] =
{
    FXMAPFUNC(SEL_CONFIGURE, 0, FXFinderMainWindow::onConfigure)
};

FXIMPLEMENT(FXFinderMainWindow, FXMainWindow, FXFinderMainWindowMap,
            ARRAYNUMBER(FXFinderMainWindowMap))

