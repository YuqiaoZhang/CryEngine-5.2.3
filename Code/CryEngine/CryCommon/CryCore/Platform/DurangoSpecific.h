// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

// -------------------------------------------------------------------------
//  File name:   Durangospecific.h
//  Version:     v1.00
//  Created:     31/03/2003 by Sergiy.
//  Compilers:   Visual Studio.NET
//  Description: Specific to Win32 declarations, inline functions etc.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#pragma once

// Ensure WINAPI version is consistent everywhere
#define _WIN32_WINNT  0x0602
#define NTDDI_VERSION 0x06020000
#define WINVER        0x0602

// Check that WINAPI_FAMILY is set appropriately
#include <winapifamily.h>
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY != WINAPI_FAMILY_TV_TITLE)
	#error WINAPI_FAMILY must be set to WINAPI_FAMILY_TV_TITLE for Durango compilation
#endif

#include <intsafe.h>

#define RC_EXECUTABLE "rc.exe"
#define SIZEOF_PTR    8

#pragma warning( disable : 4267 ) //warning C4267: 'initializing' : conversion from 'size_t' to 'unsigned int', possible loss of data

//////////////////////////////////////////////////////////////////////////
// Standard includes.
//////////////////////////////////////////////////////////////////////////
#include <malloc.h>
#include <io.h>
#include <new.h>
#include <direct.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <CryCore/BaseTypes.h>

//////////////////////////////////////////////////////////////////////////

// Special intrinsics
#include <math.h> // Should be included before intrin.h
#include <intrin.h>

#include <CryCore/Assert/CryAssert.h>

//////////////////////////////////////////////////////////////////////////
// Define platform independent types.
//////////////////////////////////////////////////////////////////////////
#include <CryCore/BaseTypes.h>

typedef unsigned char                 BYTE;

typedef unsigned long                 DWORD;

typedef void*                         THREAD_HANDLE;
typedef void*                         EVENT_HANDLE;

typedef __int64 INT_PTR, *            PINT_PTR;
typedef unsigned __int64 UINT_PTR, *  PUINT_PTR;

typedef __int64 LONG_PTR, *           PLONG_PTR;
typedef unsigned __int64 ULONG_PTR, * PULONG_PTR;

typedef ULONG_PTR DWORD_PTR, *        PDWORD_PTR;

#define stricmp  _stricmp
#define strnicmp _strnicmp
#define ltoa     _ltoa
#define strlwr   _strlwr
#define itoa     _itoa
#define strcmpi  _strcmpi
#define memicmp  _memicmp
#define wcsicmp  _wcsicmp
#define ultoa    _ultoa
#define fileno   _fileno

//typedef double              real;  //!< Biggest float-type on this machine.
typedef float real;               //!< Biggest float-type on this machine.
#define REAL_IS_FLOAT 1

#define SIZEOF_PTR    8

#ifndef FILE_ATTRIBUTE_NORMAL
	#define FILE_ATTRIBUTE_NORMAL 0x00000080
#endif

#define TARGET_DEFAULT_ALIGN (0x8U)

#define THREADID_NULL        0

typedef unsigned long int threadID;

enum EPLM_State
{
	EPLM_UNDEFINED = 0,
	EPLM_RUNNING,
	EPLM_CONSTRAINED,
	EPLM_SUSPENDED,
	EPLM_NOTRUNNING,
	EPLM_TERMINATED,
};

enum EPLM_Event
{
	EPLMEV_NONE = 0,
	EPLMEV_ON_APP_ACTIVATED,
	EPLMEV_ON_CONSTRAINED,
	EPLMEV_ON_FULL,  //!< When going from constrained back to full resources.
	EPLMEV_ON_SUSPENDING,
	EPLMEV_ON_RESUMING,
	EPLMEV_ON_TERMINATED,
};

// In RELEASE disable printf and fprintf
#include <stdio.h>
#if defined(_RELEASE) && !defined(RELEASE_LOGGING)
	#undef printf
	#define printf(...)  (void) 0
	#undef fprintf
	#define fprintf(...) (void) 0
#endif
