#include "BmlDataParser.h"
#include "pch.h"

BmlDataParser::BmlDataParser(const uint16_t ServiceID,
                             const uint8_t ComponentID,
                             const BmlModuleHandler* pHandler)
    : PSIStreamTable(true, true),
      m_DII(this),
      m_DDB(this),
      m_ServiceID(ServiceID),
      m_ComponentID(ComponentID),
      m_pHandler(pHandler) {}

bool BmlDataParser::OnTableUpdate(const PSISection* pCurSection) {
  const uint16_t DataSize = pCurSection->GetPayloadSize();
  const uint8_t* pData = pCurSection->GetPayloadData();

  if (pCurSection->GetTableID() == 0x3B) {
    // DII
    return m_DII.ParseData(pData, DataSize);
  } else if (pCurSection->GetTableID() == 0x3C) {
    // DDB
    return m_DDB.ParseData(pData, DataSize);
  }

  return false;
}

void BmlDataParser::OnDataModule(
    const DownloadInfoIndicationParser::MessageInfo* pMessageInfo,
    const DownloadInfoIndicationParser::ModuleInfo* pModuleInfo) {
  BmlModuleInfo Info = {};
  Info.DataEventID = (pMessageInfo->DownloadID >> 28) & 15;
  Info.ComponentID = m_ComponentID;
  Info.ServiceID = m_ServiceID;
  Info.ModuleID = pModuleInfo->ModuleID;
  Info.ModuleSize = pModuleInfo->ModuleSize;
  Info.ModuleVersion = pModuleInfo->ModuleVersion;
  Info.Compressed = pModuleInfo->ModuleDesc.Compression.Type == 0;
  if (Info.Compressed) {
    Info.CompressedSize = pModuleInfo->ModuleSize;
    Info.ModuleSize = pModuleInfo->ModuleDesc.Compression.OriginalSize;
  }
  if (pModuleInfo->ModuleDesc.Type.Length > 0) {
    Info.ContentType = std::string(pModuleInfo->ModuleDesc.Type.pText,
                                   pModuleInfo->ModuleDesc.Type.Length);
  }
  Info.ReturnToEntryFlag = false;  // TODO:

  auto it = m_ModuleMap.find(pModuleInfo->ModuleID);

  if (it == m_ModuleMap.end()) {
  } else if ((it->second->GetDownloadID() != pMessageInfo->DownloadID) ||
             (it->second->GetBlockSize() != pMessageInfo->BlockSize) ||
             (it->second->GetModuleSize() != pModuleInfo->ModuleSize) ||
             (it->second->GetModuleVersion() != pModuleInfo->ModuleVersion)) {
  }
}

void BmlDataParser::OnDataBlock(
    const DownloadDataBlockParser::DataBlockInfo* pDataBlock) {}
