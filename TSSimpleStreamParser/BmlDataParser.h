#pragma once

#include "pch.h"

#include "BmlModule.h"
#include "BmlParserFilter.h"

class BmlDataParser : public PSIStreamTable,
                      public DownloadInfoIndicationParser::EventHandler,
                      public DownloadDataBlockParser::EventHandler,
                      public BmlModule::BmlModuleDownloadHandler {
 public:
  BmlDataParser(const uint16_t ServiceID,
                const uint8_t ComponentID,
                BmlModuleHandler* pHandler);

 private:
  // PSIStreamTable
  bool OnTableUpdate(const PSISection* pCurSection) override;

  // DownloadInfoIndicationParser::EventHandler
  void OnDataModule(
      const DownloadInfoIndicationParser::MessageInfo* pMessageInfo,
      const DownloadInfoIndicationParser::ModuleInfo* pModuleInfo) override;

  // DownloadDataBlockParser::EventHandler
  void OnDataBlock(
      const DownloadDataBlockParser::DataBlockInfo* pDataBlock) override;

  // BmlModule
  void OnModuleDownload(const BmlModule* Module) override;

  DownloadInfoIndicationParser m_DII;
  DownloadDataBlockParser m_DDB;

  const uint8_t m_ServiceID;
  const uint8_t m_ComponentID;
  BmlModuleHandler* m_pHandler;

  std::map<uint16_t, std::unique_ptr<BmlModule>> m_ModuleMap;
};
