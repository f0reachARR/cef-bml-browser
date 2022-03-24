// TSDataBrowser.cpp : このファイルには 'main'
// 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <fstream>
#include <iostream>
#include "pch.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#include "SimpleApp.h"

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  CefEnableHighDPISupport();
  CefScopedSandboxInfo scoped_sandbox;
  auto sandbox_info = scoped_sandbox.sandbox_info();

  CefMainArgs main_args(hInstance);
  CefRefPtr<SimpleApp> app(new SimpleApp);

  int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
  if (exit_code >= 0) {
    // The sub-process has completed so return here.
    return exit_code;
  }

  //// Parse command-line arguments for use in this method.
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  CefSettings settings;
  settings.remote_debugging_port = 2012;

  // settings.log_severity = LOGSEVERITY_VERBOSE;

  // Initialize CEF.
  CefInitialize(main_args, settings, app.get(), sandbox_info);

  // Run the CEF message loop. This will block until CefQuitMessageLoop() is
  // called.
  CefRunMessageLoop();

  // Shut down CEF.
  CefShutdown();

  return 0;
}