// TSDataExtractCLI.cpp : このファイルには 'main'
// 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <BmlParserFilter.h>
#include <Engine/StreamSourceEngine.hpp>
#include <Filters/StreamSourceFilter.hpp>
#include <Filters/TSPacketParserFilter.hpp>
#include <LibISDB.hpp>
#include <TS/TSDownload.hpp>
#include <Utilities/Utilities.hpp>
#include <iostream>

class ISDBBmlParserEngine : public LibISDB::StreamSourceEngine,
                            public BmlModuleHandler {
  void OnModuleListUpdate(const BmlModuleInfo& Data) override {}
  void OnModuleDownload(const BmlModule* Data) override {
    std::cout.setf(std::ios::hex, std::ios::basefield);
    std::cout << "Download " << Data->GetModuleInfo().ComponentID << "/"
              << Data->GetModuleID() << " Count:" << Data->GetModules().size()
              << std::endl;
  }
};

#if defined(LIBISDB_WCHAR)
int wmain(int argc, wchar_t** argv) {
#else
int main(int argc, char** argv) {
#endif
  if (argc < 2) {
    std::cerr << "Need filename." << std::endl;
    return 1;
  }

  LibISDB::StreamSourceFilter* pSource = new LibISDB::StreamSourceFilter;
  LibISDB::TSPacketParserFilter* pParser = new LibISDB::TSPacketParserFilter;
  BmlParserFilter* pLogoDownloader = new BmlParserFilter;

  ISDBBmlParserEngine Engine;

  Engine.BuildEngine({pSource, pParser, pLogoDownloader});
  Engine.SetStartStreamingOnSourceOpen(true);

  pLogoDownloader->SetModuleHandler(&Engine);

  const LibISDB::CharType* pFileName = argv[1];
  if (!Engine.OpenSource(pFileName)) {
    std::cerr << "Failed to open file : " << pFileName << std::endl;
    return 1;
  }

  Engine.WaitForEndOfStream();
  Engine.CloseSource();
}
