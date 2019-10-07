// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

// -------------------------------------------------------------------------
//  File name:   FrameProfiler_impl.durango.h
//  Version:     v1.00
//  Created:     12/03/2013 by Axel Gneiting.
//  Compilers:   Visual Studio.NET
//  Description: Platform Profiling Marker Implementation for Durango using PIX marker
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#if !defined(CRY_PROFILE_MARKER_IMPL_DURANGO_H_)
#define CRY_PROFILE_MARKER_IMPL_DURANGO_H_

#if !defined(_LAUNCHER) || defined(ENABLE_PROFILING_CODE)
	#define USE_PIX
	#include <pix.h>
	#pragma comment(lib, "pixEvt")

////////////////////////////////////////////////////////////////////////////
namespace CryProfile {
namespace detail {

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//! Set the Thread Name.
void SetThreadName(const char* pName)
{
	/* not needed on Durango */
}

////////////////////////////////////////////////////////////////////////////
//! Set a one Profiling Event marker.
void SetProfilingEvent(const BYTE colorId, const char* pName)
{
	PIXSetMarker(profile_colors[(int)colorId].pack_argb8888(), L"%hs", pName);
}

////////////////////////////////////////////////////////////////////////////
//! Set the beginning of a profile marker.
void PushProfilingMarker(const BYTE colorId, const char* pName)
{
	PIXBeginEvent(profile_colors[(int)colorId].pack_argb8888(), L"%hs", pName);
}

////////////////////////////////////////////////////////////////////////////
//! Set the end of a profiling marker.
void PopProfilingMarker()
{
	PIXEndEvent();
}

void ProfilerPause()                  {}
void ProfilerResume()                 {}
void ProfilerFrameStart(int nFrameId) {}
void ProfilerFrameEnd(int nFrameId)   {}
} // namespace detail
} // namespace CryProfile

#endif

#endif // CRY_PROFILE_MARKER_IMPL_DURANGO_H_
