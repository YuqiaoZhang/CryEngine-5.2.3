// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

// -------------------------------------------------------------------------
//  File name:   FrameProfiler_impl.orbis.h
//  Version:     v1.00
//  Created:     4/4/2013 by Charlie Cole
//  Compilers:   Visual Studio.NET
//  Description: Platform Profiling Marker Implementation for Orbis
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#if !defined(CRY_PROFILE_MARKER_IMPL_ORBIS_H_)
#define CRY_PROFILE_MARKER_IMPL_ORBIS_H_

#include <CrySystem/ISystem.h>
#include <perf.h>

////////////////////////////////////////////////////////////////////////////
namespace CryProfile {
namespace detail {

#define COLOR_TO_SCE_RAZOR_COLOR(color) (0x80000000 | color.pack_bgr888())

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//! Set the Thread Name.
void SetThreadName(const char* pName)
{
	//TODO
}

////////////////////////////////////////////////////////////////////////////
//! Set a one Profiling Event marker.
void SetProfilingEvent(const BYTE colorId, const char* pName)
{
#if !defined(_RELEASE) || defined(PERFORMANCE_BUILD)
	if (gEnv->bPS4DevKit)
	{
		sceRazorCpuPushMarker(pName, COLOR_TO_SCE_RAZOR_COLOR(profile_colors[(int)colorId]), SCE_RAZOR_MARKER_ENABLE_HUD);
		sceRazorCpuPopMarker();
	}
#endif
}

////////////////////////////////////////////////////////////////////////////
//! Set the beginning of a profile marker.
void PushProfilingMarker(const BYTE colorId, const char* pName)
{
#if !defined(_RELEASE) || defined(PERFORMANCE_BUILD)
	if (gEnv->bPS4DevKit)
	{
		sceRazorCpuPushMarker(pName, COLOR_TO_SCE_RAZOR_COLOR(profile_colors[(int)colorId]), SCE_RAZOR_MARKER_ENABLE_HUD);
	}
#endif
}

////////////////////////////////////////////////////////////////////////////
//! Set the end of a profiling marker.
void PopProfilingMarker()
{
#if !defined(_RELEASE) || defined(PERFORMANCE_BUILD)
	if (gEnv->bPS4DevKit)
	{
		sceRazorCpuPopMarker();
	}
#endif
}

void ProfilerPause()                  {}
void ProfilerResume()                 {}
void ProfilerFrameStart(int nFrameId) {}
void ProfilerFrameEnd(int nFrameId)   {}
} // namespace detail
} // namespace CryProfile

#endif // CRY_PROFILE_MARKER_IMPL_ORBIS_H_
