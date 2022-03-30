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

class BmlModuleHandler {
 public:
  virtual ~BmlModuleHandler() = default;

  virtual void OnModuleListUpdate(const BmlModuleInfo& Data) = 0;
  virtual void OnModuleDownload(const BmlModuleData& Data) = 0;
};

class BmlParserFilter : public LibISDB::SingleIOFilter {
 public:
  BmlParserFilter();

  const CharType* GetObjectName() const noexcept override {
    return LIBISDB_STR("BmlDownloadFilter");
  }

  // FilterBase
  void Reset() override;

  // SingleIOFilter
  bool ProcessData(DataStream* pData) override;

 private:
  typedef std::pair<uint16_t, uint8_t> PIDToComponentMapItem;
  struct ServiceInfo {
    uint16_t ServiceID;
    uint16_t PMTPID;
    std::map<uint16_t, uint8_t> PIDToComponentMap;
  };

  LibISDB::PIDMapManager m_PIDMapManager;
  std::vector<ServiceInfo> m_ServiceList;
  BmlModuleHandler* m_pBmlHandler;

  // Handlers
  void SetModuleHandler(BmlModuleHandler* pBmlHandler);

  // Basic packet processor
  void OnPATSection(const PSITableBase* pTable, const PSISection* pSection);
  void OnPMTSection(const PSITableBase* pTable, const PSISection* pSection);

  // PMT parser utils
  int GetServiceIndexByID(uint16_t ServiceID) const;
  bool MapDataES(int Index);
  bool UnmapDataES(int Index);
};
