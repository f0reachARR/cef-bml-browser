#pragma once

#include "BmlDataParser.h"
#include "pch.h"

class BmlModule : public DataModule {
 public:
  typedef struct {
    std::string ContentLocation;
    std::string ContentType;
    const char* pData;
    uint32_t DataSize;
  } ModuleData;

  class BmlModuleHandler {
   public:
    virtual void OnModuleDownload(const BmlModule* Module,
                                  const std::vector<ModuleData>& Data) = 0;
  };

  BmlModule(uint32_t DownloadID,
            uint16_t BlockSize,
            BmlModuleInfo* Info,
            BmlModuleHandler* Handler);

 private:
  BmlModuleHandler* m_Handler;
  BmlModuleInfo* m_Info;
  void OnComplete(const uint8_t* pData, uint32_t ModuleSize) override;
};
