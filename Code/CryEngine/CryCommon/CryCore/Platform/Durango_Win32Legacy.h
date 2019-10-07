// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

// Dec2012 XDK: MoveFileEx still in header but there's no MoveFileExA in kernelx.lib
#undef MoveFileEx
BOOL MoveFileEx(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, DWORD dwFlags);

// Special defines for legacy winapi calls
#undef MoveFile
#define MoveFile(lpExistingFileName, lpNewFileName) \
  MoveFileEx(lpExistingFileName, lpNewFileName, NULL)

#undef CreateSemaphore
#define CreateSemaphore(attr, initCount, maxCount, name) \
  CreateSemaphoreExW(attr, initCount, maxCount, name, NULL, SEMAPHORE_ALL_ACCESS | SEMAPHORE_MODIFY_STATE)

#undef CreateEvent
#define CreateEvent(attr, bManualReset, bInitialState, name)                  \
  CreateEventEx(attr, name, (bManualReset ? CREATE_EVENT_MANUAL_RESET : NULL) \
                | (bInitialState ? CREATE_EVENT_INITIAL_SET : NULL), EVENT_ALL_ACCESS | EVENT_MODIFY_STATE)

#undef CreateMutex
#define CreateMutex(attr, bInitialOwner, name)                                 \
  CreateMutexEx(attr, name, bInitialOwner ? CREATE_MUTEX_INITIAL_OWNER : NULL, \
                MUTEX_ALL_ACCESS)

#define MAX_COMPUTERNAME_LENGTH 15 // required by CryOnline module
