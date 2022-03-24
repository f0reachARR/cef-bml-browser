// TSDataBrowser.cpp : このファイルには 'main'
// 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <fstream>
#include <iostream>
#include "pch.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

void CheckBML(rapidxml::xml_node<>* node) {
  auto first_node = node->first_node();
  while (first_node) {
    CheckBML(first_node);
    first_node = first_node->next_sibling();
  }

  if (node->type() == rapidxml::node_type::node_cdata) {
    node->type(rapidxml::node_data);
  }

  if (node->value_size() == 0 && !node->first_node()) {
    node->append_node(
        node->document()->allocate_node(rapidxml::node_data, "", ""));
  }
}

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  std::ifstream input("../x64/Debug/sampledata/40/0000/startup.bml",
                      std::ios::binary);

  std::ofstream output("../x64/Debug/modified.bml", std::ios::binary);

  if (input) {
    std::stringstream buffer;
    buffer << input.rdbuf();

    input.close();

    std::string xml = buffer.str();

    rapidxml::xml_document<char> doc;
    try {
      doc.parse<0>(&xml[0]);
    } catch (rapidxml::parse_error& ex) {
      int position = (int)ex.where<char>() - (int)&xml[0];

      LOG(INFO) << "parse error (" << position << ") : " << ex.what()
                << std::endl;
      return false;
    }

    CheckBML(doc.first_node());

    rapidxml::print(std::ostream_iterator<char>(output), doc,
                    rapidxml::print_script_noexpand);
  }
  // CefEnableHighDPISupport();
  // CefScopedSandboxInfo scoped_sandbox;
  // auto sandbox_info = scoped_sandbox.sandbox_info();

  // CefMainArgs main_args(hInstance);
  // CefRefPtr<SimpleApp> app(new SimpleApp);

  // int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
  // if (exit_code >= 0) {
  //  // The sub-process has completed so return here.
  //  return exit_code;
  //}

  //// Parse command-line arguments for use in this method.
  // CefRefPtr<CefCommandLine> command_line =
  // CefCommandLine::CreateCommandLine();
  // command_line->InitFromString(::GetCommandLineW());

  // CefSettings settings;
  // settings.remote_debugging_port = 2012;

  //// settings.log_severity = LOGSEVERITY_VERBOSE;

  //// Initialize CEF.
  // CefInitialize(main_args, settings, app.get(), sandbox_info);

  //// Run the CEF message loop. This will block until CefQuitMessageLoop() is
  //// called.
  // CefRunMessageLoop();

  //// Shut down CEF.
  // CefShutdown();

  return 0;
}