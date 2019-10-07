// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#if defined(USE_CRY_ASSERT) && CRY_PLATFORM_ORBIS

#include <stdio.h>
#include <stdarg.h>
#include <libdbg.h>

static THREADLOCAL char gAssertMessageBuffer[256];

void CryAssertTrace(const char* szFormat, ...)
{
	if (szFormat)
	{
		va_list args;
		va_start(args, szFormat);
		cry_vsprintf(gAssertMessageBuffer, szFormat, args);
		va_end(args);
	}
	else
	{
		cry_strcpy(gAssertMessageBuffer, "<no message was provided for this assert>");
	}
}

bool CryAssert(const char* szCondition, const char* szFile, unsigned int line, bool* pIgnoreThisAssert)
{
	IF_UNLIKELY (!gEnv || !gEnv->pSystem || !gEnv->pLog)
	{
		return false;
	}

	char buffer[MAX_PATH];
	char simplified[MAX_PATH];
	const char* const szSimplifiedFile = PathUtil::SimplifyFilePath(szFile, simplified, MAX_PATH, PathUtil::ePathStyle_Windows) ? simplified : szFile;
	cry_sprintf(buffer, "Assert failed in %s (line %d): %s %s\n", szSimplifiedFile, line, szCondition, gAssertMessageBuffer);
	CryLogAlways("%s", buffer);

	gEnv->pSystem->OnAssert(szCondition, gAssertMessageBuffer, szFile, line);

	return false;
}

#endif
