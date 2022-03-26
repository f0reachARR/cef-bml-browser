// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#define TVTEST_PLUGIN_CLASS_IMPLEMENT
#include "TSDataViewer.h"

TVTest::CTVTestPlugin* CreatePluginClass() {
  return new TSDataViewer();
}