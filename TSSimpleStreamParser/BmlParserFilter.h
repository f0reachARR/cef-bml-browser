#pragma once

#include "BmlModule.h"
#include "pch.h"

class BmlModuleHandler {
 public:
  virtual ~BmlModuleHandler() = default;

  virtual void OnModuleListUpdate(const BmlModuleInfo& Data) = 0;
  virtual void OnModuleDownload(const BmlModule& Data) = 0;
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

  // Handlers
  void SetModuleHandler(BmlModuleHandler* pBmlHandler);

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

  // Basic packet processor
  void OnPATSection(const PSITableBase* pTable, const PSISection* pSection);
  void OnPMTSection(const PSITableBase* pTable, const PSISection* pSection);

  // PMT parser utils
  int GetServiceIndexByID(uint16_t ServiceID) const;
  bool MapDataES(int Index);
  bool UnmapDataES(int Index);
};
