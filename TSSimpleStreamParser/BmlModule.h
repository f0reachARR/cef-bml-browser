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
    const uint8_t* pData;
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

  const std::vector<ModuleData>& GetModules() const { return m_Modules; }
  const BmlModuleInfo& GetModuleInfo() const { return m_Info; }

 private:
  BmlModuleDownloadHandler* m_Handler;
  BmlModuleInfo m_Info;
  std::vector<ModuleData> m_Modules;
  uint8_t* m_pUncompressedBuffer;

  void OnComplete(const uint8_t* pData, uint32_t ModuleSize) override;

  std::string ReadLine(const uint8_t** pData, uint32_t& RemainSize);
  std::string ReadBoundary(const std::string& ContentType);
  void ReadHeader(const uint8_t** pData,
                  uint32_t& RemainSize,
                  std::map<const std::string, const std::string>& Headers);
};
