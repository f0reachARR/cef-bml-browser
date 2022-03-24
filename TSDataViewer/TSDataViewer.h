#pragma once

#include "pch.h"

class TSDataViewer :
    public TVTest::CTVTestPlugin
{
	bool GetPluginInfo(TVTest::PluginInfo* pInfo);
	bool Initialize();
	bool Finalize();
};

