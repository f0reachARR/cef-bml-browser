#pragma once

#include "pch.h"

struct BmlModuleInfo {
  uint16_t ServiceID;
  uint16_t ComponentID;
  uint16_t DataEventID;
  uint16_t ModuleID;
  uint16_t ModuleVersion;
  uint32_t ModuleSize;
  uint32_t CompressedSize;
  std::string ContentType;
  bool Compressed;
  bool ReturnToEntryFlag;
};

class BmlModule : public DataModule {
 public:
  typedef struct {
    std::string ContentLocation;
    std::string ContentType;
    const char* pData;
    uint32_t DataSize;
  } ModuleData;

  class BmlModuleDownloadHandler {
   public:
    virtual void OnModuleDownload(const BmlModule* Module) = 0;
  };

  BmlModule(uint32_t DownloadID,
            uint16_t BlockSize,
            BmlModuleInfo* Info,
            BmlModuleDownloadHandler* Handler);

 private:
  BmlModuleDownloadHandler* m_Handler;
  BmlModuleInfo m_Info;
  std::vector<ModuleData> m_Modules;

  void OnComplete(const uint8_t* pData, uint32_t ModuleSize) override;
};
