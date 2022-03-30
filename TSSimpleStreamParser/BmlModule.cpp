#include "BmlModule.h"
#include "pch.h"

BmlModule::BmlModule(uint32_t DownloadID,
                     uint16_t BlockSize,
                     BmlModuleInfo* Info,
                     BmlModuleHandler* Handler)
    : DataModule(DownloadID,
                 BlockSize,
                 Info->ModuleID,
                 Info->ModuleSize,
                 Info->ModuleVersion),
      m_Info(Info),
      m_Handler(Handler) {}

void BmlModule::OnComplete(const uint8_t* pData, uint32_t ModuleSize) {
  // TODO: Parse module
  std::vector<ModuleData> ModuleList;
  m_Handler->OnModuleDownload(this, ModuleList);
}
