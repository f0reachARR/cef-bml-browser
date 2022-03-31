#include "BmlParserFilter.h"
#include "BmlDataParser.h"
#include "pch.h"

BmlParserFilter::BmlParserFilter() {
  Reset();
}

void BmlParserFilter::Reset() {
  BlockLock lock(m_FilterLock);

  m_PIDMapManager.UnmapAllTargets();

  m_PIDMapManager.MapTarget(PID_PAT, PSITableBase::CreateWithHandler<PATTable>(
                                         &BmlParserFilter::OnPATSection, this));

  m_PIDMapManager.MapTarget(PID_TOT, new TOTTable);
}

bool BmlParserFilter::ProcessData(DataStream* pData) {
  if (pData->Is<TSPacket>())
    m_PIDMapManager.StorePacketStream(pData);

  return true;
}

void BmlParserFilter::SetModuleHandler(BmlModuleHandler* pBmlHandler) {
  m_pBmlHandler = pBmlHandler;
}

void BmlParserFilter::OnPATSection(const PSITableBase* pTable,
                                   const PSISection* pSection) {
  const PATTable* pPATTable = dynamic_cast<const PATTable*>(pTable);
  if (pPATTable == nullptr)
    return;

  for (size_t i = 0; i < m_ServiceList.size(); i++) {
    m_PIDMapManager.UnmapTarget(m_ServiceList[i].PMTPID);
    UnmapDataES(static_cast<int>(i));
  }

  m_ServiceList.resize(pPATTable->GetProgramCount());

  for (size_t i = 0; i < m_ServiceList.size(); i++) {
    const uint16_t PMTPID = pPATTable->GetPMTPID(static_cast<int>(i));

    m_ServiceList[i].ServiceID =
        pPATTable->GetProgramNumber(static_cast<int>(i));
    m_ServiceList[i].PMTPID = PMTPID;
    m_ServiceList[i].PIDToComponentMap.clear();

    m_PIDMapManager.MapTarget(PMTPID,
                              PSITableBase::CreateWithHandler<PMTTable>(
                                  &BmlParserFilter::OnPMTSection, this));
  }
}

void BmlParserFilter::OnPMTSection(const PSITableBase* pTable,
                                   const PSISection* pSection) {
  const PMTTable* pPMTTable = dynamic_cast<const PMTTable*>(pTable);
  if (pPMTTable == nullptr)
    return;

  const int ServiceIndex = GetServiceIndexByID(pPMTTable->GetProgramNumberID());
  if (ServiceIndex < 0)
    return;
  ServiceInfo& Info = m_ServiceList[ServiceIndex];

  // data PID list maybe updated, so clear it before
  UnmapDataES(ServiceIndex);
  Info.PIDToComponentMap.clear();

  for (uint16_t ESIndex = 0; ESIndex < pPMTTable->GetESCount(); ESIndex++) {
    if (pPMTTable->GetStreamType(ESIndex) == STREAM_TYPE_DATA_CARROUSEL) {
      const DescriptorBlock* pDescBlock =
          pPMTTable->GetItemDescriptorBlock(ESIndex);
      if (pDescBlock != nullptr) {
        const StreamIDDescriptor* pStreamIDDesc =
            pDescBlock->GetDescriptor<StreamIDDescriptor>();
        const DataComponentDescriptor* pDataComponentDesc =
            pDescBlock->GetDescriptor<DataComponentDescriptor>();

        if (pStreamIDDesc != nullptr && pDataComponentDesc != nullptr) {
          auto PID = pPMTTable->GetESPID(ESIndex);
          auto ComponentID = pStreamIDDesc->GetComponentTag();

          Info.PIDToComponentMap.emplace(std::pair(PID, ComponentID));
        }
      }
    }
  }

  // Re-register data PID list
  MapDataES(ServiceIndex);
}

int BmlParserFilter::GetServiceIndexByID(uint16_t ServiceID) const {
  for (size_t i = 0; i < m_ServiceList.size(); i++) {
    if (m_ServiceList[i].ServiceID == ServiceID)
      return static_cast<int>(i);
  }
  return -1;
}

bool BmlParserFilter::MapDataES(int Index) {
  if (static_cast<unsigned int>(Index) >= m_ServiceList.size())
    return false;

  ServiceInfo& Info = m_ServiceList[Index];

  for (PIDToComponentMapItem Mapper : Info.PIDToComponentMap) {
    m_PIDMapManager.MapTarget(
        Mapper.first,
        new BmlDataParser(Info.ServiceID, Mapper.second, m_pBmlHandler));
  }

  return true;
}

bool BmlParserFilter::UnmapDataES(int Index) {
  if (static_cast<unsigned int>(Index) >= m_ServiceList.size())
    return false;

  ServiceInfo& Info = m_ServiceList[Index];
  for (PIDToComponentMapItem Mapper : Info.PIDToComponentMap) {
    m_PIDMapManager.UnmapTarget(Mapper.first);
  }

  return true;
}
