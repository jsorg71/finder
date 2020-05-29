/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2019-2020
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

#if !defined(FINDER_GUI_ABOUT_H)
#define FINDER_GUI_ABOUT_H

class AboutDialog : public FXDialogBox
{
    FXDECLARE(AboutDialog)
public:
    AboutDialog();
    AboutDialog(FXApp* app, FXWindow* parent, struct finder_info* fi);
    virtual ~AboutDialog();
    long onPress(FXObject* obj, FXSelector sel, void* ptr);
public:
    enum _ids
    {
        ID_BUTTON = FXDialogBox::ID_LAST,
        ID_LAST
    };
public:
    FXApp* m_app;
    FXButton* m_ok_but;
    FXLabel* m_text;
    struct finder_info* m_fi;
};

#endif

