// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include "TSDataViewer.h"

TVTest::CTVTestPlugin* CreatePluginClass() {
	return new TSDataViewer();
}