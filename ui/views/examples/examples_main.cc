// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "examples_window.h"
#include "ui/base/resource/resource_bundle.h"
#include "base/command_line.h"
#include "base/at_exit.h"
#include "base/i18n/icu_util.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "ui/base/win/scoped_ole_initializer.h"
#include "ui/base/ui_base_paths.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, wchar_t*, int) {
    base::AtExitManager exit_manager;
    int argc = 0;
    char** argv = NULL;
    CommandLine::Init(argc, argv);

    base::i18n::InitializeICU();
    scoped_ptr<ui::ScopedOleInitializer> ole_initializer;
    ole_initializer.reset(new ui::ScopedOleInitializer);

    base::MessageLoop message_loop(base::MessageLoop::TYPE_UI);

    ui::RegisterPathProvider();
    ResourceBundle::InitSharedInstanceWithLocale("en-US", NULL);
    ShowExamplesWindow(views::examples::QUIT_ON_CLOSE);

    base::RunLoop run_loop;
    run_loop.Run();
  return 0;
}
