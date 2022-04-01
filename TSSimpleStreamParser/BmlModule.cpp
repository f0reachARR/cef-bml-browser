#include "BmlModule.h"
#include "pch.h"

BmlModule::BmlModule(uint32_t DownloadID,
                     uint16_t BlockSize,
                     BmlModuleInfo* Info,
                     BmlModuleDownloadHandler* Handler)
    : DataModule(DownloadID,
                 BlockSize,
                 Info->ModuleID,
                 Info->Compressed ? Info->CompressedSize : Info->ModuleSize,
                 Info->ModuleVersion),
      m_Info(*Info),
      m_Handler(Handler),
      m_pUncompressedBuffer(nullptr) {}

void BmlModule::OnComplete(const uint8_t* pData, uint32_t ModuleSize) {
  // TODO: Parse module
  m_Modules.clear();

  if (m_Info.Compressed) {
    m_pUncompressedBuffer = (uint8_t*)malloc(m_Info.ModuleSize);
    z_stream zst;
    zst.zalloc = (alloc_func)0;
    zst.zfree = (free_func)0;
    zst.next_in = (Bytef*)pData;
    zst.avail_in = m_Info.CompressedSize;
    zst.next_out = (Bytef*)m_pUncompressedBuffer;
    zst.avail_out = m_Info.ModuleSize;
    int z_err = inflateInit(&zst);
    if (z_err == Z_OK) {
      z_err = inflate(&zst, Z_FINISH);
      if (z_err != Z_STREAM_END) {
        inflateEnd(&zst);
      }
      z_err = inflateEnd(&zst);
    } else {
      return;
    }

    pData = m_pUncompressedBuffer;
    ModuleSize = m_Info.ModuleSize;
  }

  std::map<const std::string, const std::string> Headers;
  ReadHeader(&pData, ModuleSize, Headers);
  auto it = Headers.find("Content-Type");
  if (it == Headers.end()) {
    LibISDB::DebugTrace(TraceType::Error,
                        LIBISDB_STR("Missing Content-Type. ModuleID: %d"),
                        m_Info.ModuleID);
    return;
  }

  auto ContentType = it->second;

  if (ContentType.compare(0, 15, "multipart/mixed") == 0) {
    // Multipart
    auto Boundary = "--" + ReadBoundary(ContentType);
    while (ModuleSize > 0) {
      // Boundary check
      auto Line = ReadLine(&pData, ModuleSize);
      if (Line.compare(0, Boundary.size(), Boundary) != 0) {
        if (Line.empty())
          continue;
        LibISDB::DebugTrace(
            TraceType::Error,
            LIBISDB_STR("Invalid boundary detected. ModuleID: %d"),
            m_Info.ModuleID);
        break;
      }

      if (ModuleSize == 0)
        break;

      // Read part headers
      Headers.clear();
      ReadHeader(&pData, ModuleSize, Headers);
      if (Headers.size() == 0) {
        // No header
        LibISDB::DebugTrace(TraceType::Error,
                            LIBISDB_STR("No header detected. ModuleID: %d"),
                            m_Info.ModuleID);
        break;
      }

      it = Headers.find("Content-Length");
      if (it == Headers.end()) {
        // Unknown length
        LibISDB::DebugTrace(
            TraceType::Error,
            LIBISDB_STR("Content length is unknown. ModuleID: %d"),
            m_Info.ModuleID);
        break;
      }

      auto Length = atoi(it->second.c_str());

      it = Headers.find("Content-Type");
      auto ContentType = it == Headers.end() ? "" : it->second;

      it = Headers.find("Content-Location");
      auto ContentLocation = it == Headers.end() ? "" : it->second;

      ModuleData Data = {ContentLocation, ContentType, pData, Length};
      m_Modules.emplace_back(Data);

      pData += Length;
      ModuleSize -= Length;
    }

  } else {
    // Not multipart
    it = Headers.find("Content-Location");
    auto ContentLocation = it == Headers.end() ? "" : it->second;
    ModuleData Data = {ContentLocation, ContentType, pData, ModuleSize};
    m_Modules.emplace_back(Data);
  }
  m_Handler->OnModuleDownload(this);
}

std::string BmlModule::ReadLine(const uint8_t** pData, uint32_t& RemainSize) {
  auto* pBegin = *pData;
  auto Count = 0;
  while (Count < RemainSize && **pData != '\0' && **pData != '\n' &&
         **pData != '\r') {
    Count++;
    (*pData)++;
  }
  RemainSize -= Count;

  if (**pData == '\r' && *(*pData + 1) == '\n') {
    (*pData)++;
    RemainSize--;
  }

  (*pData)++;
  RemainSize--;

  return std::string((char*)pBegin, Count);
}

std::string BmlModule::ReadBoundary(const std::string& ContentType) {
  auto BoundaryStart = ContentType.find("boundary=") + 9;
  if (ContentType[BoundaryStart] == '"')
    BoundaryStart++;
  auto BoundaryEnd = ContentType.find_first_of('"', BoundaryStart + 1);
  if (BoundaryEnd == std::string::npos) {
    return ContentType.substr(BoundaryStart);
  } else {
    return ContentType.substr(BoundaryStart, BoundaryEnd - BoundaryStart);
  }
}

void BmlModule::ReadHeader(
    const uint8_t** pData,
    uint32_t& RemainSize,
    std::map<const std::string, const std::string>& Headers) {
  std::string Line;
  do {
    Line = ReadLine(pData, RemainSize);
    auto DividerPos = Line.find_first_of(':');

    if (DividerPos == std::string::npos)
      break;

    auto Name = Line.substr(0, DividerPos);
    auto Value = Line.substr(DividerPos + 1);
    Value.erase(0, Value.find_first_not_of(' '));

    Headers.emplace(std::pair(Name, Value));
  } while (!Line.empty());
}
