#include "TSDataViewer.h"

bool TSDataViewer::GetPluginInfo(TVTest::PluginInfo* pInfo) {
  pInfo->pszPluginName = L"TSDataViewer";
  pInfo->pszCopyright = L"f0reachARR";
  pInfo->pszDescription = L"View data broadcasting with Chromium";
  pInfo->Flags = 0;
  pInfo->Type = TVTest::PLUGIN_TYPE_NORMAL;
  return true;
}

bool TSDataViewer::Initialize() {
  // this->m_pApp->SetWindowMessageCallback()
  // this->m_pApp->SetStreamCallback(0, StreamCallback, this);
  return true;
}

bool TSDataViewer::Finalize() {
  return true;
}

BOOL TSDataViewer::StreamCallback(BYTE* pData, void* pClientData) {
  return TRUE;
}
