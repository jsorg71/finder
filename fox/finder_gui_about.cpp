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

#include "finder.h"
#include "finder_gui_about.h"

FXDEFMAP(AboutDialog) AboutDialogMap[] =
{
    FXMAPFUNC(SEL_COMMAND, AboutDialog::ID_BUTTON, AboutDialog::onPress)
};

FXIMPLEMENT(AboutDialog, FXDialogBox, AboutDialogMap, ARRAYNUMBER(AboutDialogMap))

/*****************************************************************************/
AboutDialog::AboutDialog() : FXDialogBox()
{
    m_ok_but = NULL;
    m_app = NULL;
    m_fi = NULL;
}

/*****************************************************************************/
AboutDialog::AboutDialog(FXApp* app, FXWindow* parent, struct finder_info* fi) : FXDialogBox(parent, "Finder About")
{
    FXuint flags;
    FXSelector sel;
    FXString ver;

    writeln(m_fi, "AboutDialog::AboutDialog");
    setWidth(400);
    setHeight(100);
    flags = BUTTON_NORMAL | LAYOUT_EXPLICIT | BUTTON_DEFAULT;
    sel = AboutDialog::ID_BUTTON;
    m_ok_but = new FXButton(this, "&Ok", NULL, this, sel, flags, 300, 60, 80, 30);
    flags = LABEL_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    ver.format("Finder version %d.%d", FINDER_VERSION_MAJOR, FINDER_VERSION_MINOR);
    m_text = new FXLabel(this, ver, NULL, flags, 10, 10, 300, 30);
    m_app = app;
    m_fi = fi;
    m_ok_but->setFocus();
}

/*****************************************************************************/
AboutDialog::~AboutDialog()
{
    writeln(m_fi, "AboutDialog::~AboutDialog");
}

/*****************************************************************************/
long
AboutDialog::onPress(FXObject* obj, FXSelector sel, void* ptr)
{
    writeln(m_fi, "AboutDialog::onPress");
    if (obj == m_ok_but)
    {
        return onCmdAccept(obj, sel, ptr);
    }
    return onCmdCancel(obj, sel, ptr);
}
