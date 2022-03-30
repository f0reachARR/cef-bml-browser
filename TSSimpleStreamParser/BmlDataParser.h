#pragma once

#include "BmlModule.h"
#include "BmlParserFilter.h"
#include "pch.h"

class BmlDataParser : public PSIStreamTable,
                      public DownloadInfoIndicationParser::EventHandler,
                      public DownloadDataBlockParser::EventHandler,
{
 public:
  BmlDataParser(const uint16_t ServiceID,
                const uint8_t ComponentID,
                const BmlModuleHandler* pHandler);

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

  DownloadInfoIndicationParser m_DII;
  DownloadDataBlockParser m_DDB;

  const uint8_t m_ServiceID;
  const uint8_t m_ComponentID;
  const BmlModuleHandler* m_pHandler;

  std::map<uint16_t, std::unique_ptr<BmlModule>> m_ModuleMap;
};