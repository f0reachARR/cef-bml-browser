#pragma once

#include "framework.h"

#include "TVTestPlugin.h"

class TSDataViewer : public TVTest::CTVTestPlugin {
 public:
  bool GetPluginInfo(TVTest::PluginInfo* pInfo);
  bool Initialize();
  bool Finalize();

 private:
  static BOOL CALLBACK StreamCallback(BYTE* pData, void* pClientData);
};
