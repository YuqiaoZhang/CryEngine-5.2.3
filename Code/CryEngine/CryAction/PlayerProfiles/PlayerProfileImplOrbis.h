// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

//
////////////////////////////////////////////////////////////////////////////

#if CRY_PLATFORM_ORBIS

#pragma once

#include "PlayerProfileImplConsole.h"

class CPlayerProfileImplOrbis : public CPlayerProfileImplConsole
{

private:
	// CPlayerProfileImplConsole
	virtual void Release();
	virtual void GetMemoryStatistics(ICrySizer* s);
	// ~CPlayerProfileImplConsole

	// ICommonProfileImpl
	virtual void InternalMakeFSPath(SUserEntry* pEntry, const char* profileName, string& outPath);
	virtual void InternalMakeFSSaveGamePath(SUserEntry* pEntry, const char* profileName, string& outPath, bool bNeedFolder);
	// ~ICommonProfileImpl
};
#endif
