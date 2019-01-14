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

#include <fx.h>

#include "finder_gui_about.h"

FXDEFMAP(AboutDialog) AboutDialogMap[] =
{
};

FXIMPLEMENT(AboutDialog, FXDialogBox, AboutDialogMap, ARRAYNUMBER(AboutDialogMap))

/*****************************************************************************/
AboutDialog::AboutDialog() : FXDialogBox()
{
}

/*****************************************************************************/
AboutDialog::AboutDialog(FXApp* app, FXWindow* parent) : FXDialogBox(app, "Finder About")
{
    setWidth(400);
    setHeight(100);
}

/*****************************************************************************/
AboutDialog::~AboutDialog()
{
}
