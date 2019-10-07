// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Description:
   Assert dialog box for Durango

   -------------------------------------------------------------------------
   History:
   - 09:04:2010: Created by Ian Masters

*************************************************************************/

#if defined(USE_CRY_ASSERT) && CRY_PLATFORM_DURANGO

#include <CryRenderer/IRenderer.h>
#include <CrySystem/IConsole.h>
#include <CryCore/Platform/IPlatformOS.h>

static char gs_szMessage[MAX_PATH];

void CryAssertTrace(const char* _pszFormat, ...)
{
	if (gEnv == 0)
	{
		return;
	}
	if (NULL == _pszFormat)
	{
		gs_szMessage[0] = '\0';
	}
	else
	{
		va_list args;
		va_start(args, _pszFormat);
		cry_vsprintf(gs_szMessage, _pszFormat, args);
		va_end(args);
	}

	OutputDebugStringA(gs_szMessage);
}

bool CryAssert(const char* _pszCondition, const char* _pszFile, unsigned int _uiLine, bool* _pbIgnore)
{
	char buffer[MAX_PATH];
	cry_sprintf(buffer, "Assert failed in %s (line %d): %s\n", _pszFile, _uiLine, _pszCondition);
	OutputDebugStringA(buffer);

	if (gEnv)
	{
		gEnv->pSystem->OnAssert(_pszCondition, gs_szMessage, _pszFile, _uiLine);
	}
	return false;
}

#endif
