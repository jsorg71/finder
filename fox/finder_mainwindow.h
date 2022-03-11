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

#if !defined(FINDER_MAINWINDOW_H)
#define FINDER_MAINWINDOW_H

#define SEL_FINDERCONF (SEL_LAST + 1)

class FXFinderMainWindow:public FXMainWindow
{
    FXDECLARE(FXFinderMainWindow)
public:
    FXFinderMainWindow();
    FXFinderMainWindow(FXApp* app);
    virtual ~FXFinderMainWindow();
public:
    long onConfigure(FXObject* obj, FXSelector sel, void* ptr);
};

#endif
