// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

// -------------------------------------------------------------------------
//  File name:   Orbis_Win32Wrapper.h
//  Version:     v0.01
//  Created:     5/9/2012 by Charlie
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __Orbis_Win32Wrapper_h__
#define __Orbis_Win32Wrapper_h__
#pragma once

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <fios2.h>

/* Memory block identification */
#define _FREE_BLOCK   0
#define _NORMAL_BLOCK 1
#define _CRT_BLOCK    2
#define _IGNORE_BLOCK 3
#define _CLIENT_BLOCK 4
#define _MAX_BLOCKS   5

typedef void* HMODULE;

typedef struct _MEMORYSTATUS
{
	DWORD  dwLength;
	DWORD  dwMemoryLoad;
	SIZE_T dwTotalPhys;
	SIZE_T dwAvailPhys;
	SIZE_T dwTotalPageFile;
	SIZE_T dwAvailPageFile;
	SIZE_T dwTotalVirtual;
	SIZE_T dwAvailVirtual;
} MEMORYSTATUS, * LPMEMORYSTATUS;

extern void GlobalMemoryStatus(LPMEMORYSTATUS lpmem);

#define MEMORY_ALLOCATION_ALIGNMENT   16

#define S_OK                          0
#define THREAD_PRIORITY_IDLE          (SCE_KERNEL_PRIO_FIFO_LOWEST * 2)
#define THREAD_PRIORITY_LOWEST        (SCE_KERNEL_PRIO_FIFO_LOWEST)
#define THREAD_PRIORITY_BELOW_NORMAL  ((SCE_KERNEL_PRIO_FIFO_DEFAULT + SCE_KERNEL_PRIO_FIFO_LOWEST) / 2)
#define THREAD_PRIORITY_NORMAL        (SCE_KERNEL_PRIO_FIFO_DEFAULT)
#define THREAD_PRIORITY_ABOVE_NORMAL  ((SCE_KERNEL_PRIO_FIFO_DEFAULT + SCE_KERNEL_PRIO_FIFO_HIGHEST) / 2)
#define THREAD_PRIORITY_HIGHEST       (SCE_KERNEL_PRIO_FIFO_HIGHEST)
#define THREAD_PRIORITY_TIME_CRITICAL (1)
#define MAX_COMPUTERNAME_LENGTH       15 // required by CryOnline module

// stdlib.h stuff
#define _MAX_DRIVE 3    // max. length of drive component
#define _MAX_DIR   256  // max. length of path component
#define _MAX_FNAME 256  // max. length of file name component
#define _MAX_EXT   256  // max. length of extension component

// fcntl.h
#define _O_RDONLY      0x0000   /* open for reading only */
#define _O_WRONLY      0x0001   /* open for writing only */
#define _O_RDWR        0x0002   /* open for reading and writing */
#define _O_APPEND      0x0008   /* writes done at eof */
#define _O_CREAT       0x0100   /* create and open file */
#define _O_TRUNC       0x0200   /* open and truncate */
#define _O_EXCL        0x0400   /* open only if file doesn't already exist */
#define _O_TEXT        0x4000   /* file mode is text (translated) */
#define _O_BINARY      0x8000   /* file mode is binary (untranslated) */
#define _O_RAW         _O_BINARY
#define _O_NOINHERIT   0x0080   /* child process doesn't inherit file */
#define _O_TEMPORARY   0x0040   /* temporary file bit */
#define _O_SHORT_LIVED 0x1000   /* temporary storage file, try not to flush */
#define _O_SEQUENTIAL  0x0020   /* file access is primarily sequential */
#define _O_RANDOM      0x0010   /* file access is primarily random */

// io.h stuff
extern int errno;
typedef unsigned int _fsize_t;

struct _OVERLAPPED;
typedef _OVERLAPPED* LPOVERLAPPED;

typedef void (*      LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, struct _OVERLAPPED* lpOverlapped);

typedef struct tagRECT
{
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT, * PRECT;

typedef struct tagPOINT
{
	LONG x;
	LONG y;
} POINT, * PPOINT;

#ifndef _FILETIME_
	#define _FILETIME_
typedef struct _FILETIME
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME, * PFILETIME, * LPFILETIME;
#endif

typedef union _ULARGE_INTEGER
{
	struct
	{
		DWORD LowPart;
		DWORD HighPart;
	};
	unsigned long long QuadPart;
} ULARGE_INTEGER;

typedef ULARGE_INTEGER* PULARGE_INTEGER;

#ifdef __cplusplus
inline LONG CompareFileTime(const FILETIME* lpFileTime1, const FILETIME* lpFileTime2)
#else
static LONG CompareFileTime(const FILETIME* lpFileTime1, const FILETIME* lpFileTime2)
#endif
{
	ULARGE_INTEGER u1, u2;
	memcpy(&u1, lpFileTime1, sizeof u1);
	memcpy(&u2, lpFileTime2, sizeof u2);
	if (u1.QuadPart < u2.QuadPart)
		return -1;
	else if (u1.QuadPart > u2.QuadPart)
		return 1;
	return 0;
}

typedef struct _SYSTEMTIME
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, * PSYSTEMTIME, * LPSYSTEMTIME;

typedef struct _TIME_FIELDS
{
	short Year;
	short Month;
	short Day;
	short Hour;
	short Minute;
	short Second;
	short Milliseconds;
	short Weekday;
} TIME_FIELDS, * PTIME_FIELDS;

#define DAYSPERNORMALYEAR 365
#define DAYSPERLEAPYEAR   366
#define MONSPERYEAR       12

inline void ZeroMemory(void* pPtr, int nSize)
{
	memset(pPtr, 0, nSize);
}

inline BOOL InflateRect(RECT* pRect, int dx, int dy)
{
	pRect->left -= dx;
	pRect->right += dx;
	pRect->top -= dy;
	pRect->bottom += dy;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
extern BOOL SystemTimeToFileTime(const SYSTEMTIME* syst, LPFILETIME ft);
//! Win32API function declarations actually used.
extern bool IsBadReadPtr(void* ptr, unsigned int size);

//! Defined in the launcher.
DLL_IMPORT void OutputDebugString(const char*);

extern bool     QueryPerformanceCounter(LARGE_INTEGER* counter);
extern bool     QueryPerformanceFrequency(LARGE_INTEGER* frequency);

#define GetTickCount   CryGetTicks
#define GetCurrentTime CryGetTicks

#define IGNORE         0            // Ignore signal
#define INFINITE       0xFFFFFFFF   // Infinite timeout

extern BOOL GetUserName(LPSTR lpBuffer, LPDWORD nSize);

//! Error code stuff. Not thread specific, just a coarse implementation for the main thread.
inline DWORD GetLastError() { return errno; }

//! Error code stuff. Not thread specific, just a coarse implementation for the main thread.
inline void SetLastError(DWORD dwErrCode) { errno = dwErrCode; }

template<class T, T U>
class CHandle
{
public:
	typedef T     HandleType;
	typedef void* PointerType;
	static const HandleType sciInvalidHandleValue = U;
	CHandle(const CHandle<T, U>& cHandle) : m_Value(cHandle.m_Value){}
	CHandle(const HandleType cHandle = U) : m_Value(cHandle){}
	operator HandleType(){ return m_Value; }
	bool           operator!() const                          { return m_Value == sciInvalidHandleValue; }
	const CHandle& operator=(const CHandle& crHandle)         { m_Value = crHandle.m_Value; return *this; }
	const CHandle& operator=(const PointerType cpHandle)      { m_Value = reinterpret_cast<HandleType>(cpHandle); return *this; }
	const bool     operator==(const CHandle& crHandle) const  { return m_Value == crHandle.m_Value; }
	const bool     operator==(const HandleType cHandle) const { return m_Value == cHandle; }
	const bool     operator!=(const HandleType cHandle) const { return m_Value != cHandle; }
	const bool     operator!=(const CHandle& crHandle) const  { return m_Value != crHandle.m_Value; }
	const bool     operator<(const CHandle& crHandle) const   { return m_Value < crHandle.m_Value; }
	HandleType     Handle() const                             { return m_Value; }
private:
	HandleType m_Value;
	typedef void ReferenceType;
	PointerType   operator->();
	PointerType   operator->() const;
	ReferenceType operator*();
	ReferenceType operator*() const;
	operator PointerType();
};
typedef CHandle<INT_PTR, (INT_PTR) 0> HANDLE;
typedef HANDLE                        EVENT_HANDLE;
typedef HANDLE                        THREAD_HANDLE;

typedef struct _SECURITY_ATTRIBUTES
{
	DWORD  nLength;
	LPVOID lpSecurityDescriptor;
	BOOL   bInheritHandle;
} SECURITY_ATTRIBUTES, * PSECURITY_ATTRIBUTES, * LPSECURITY_ATTRIBUTES;

//////////////////////////////////////////////////////////////////////////
#define GENERIC_READ              (0x80000000L)
#define GENERIC_WRITE             (0x40000000L)
#define GENERIC_EXECUTE           (0x20000000L)
#define GENERIC_ALL               (0x10000000L)

#define CREATE_NEW                1
#define CREATE_ALWAYS             2
#define OPEN_EXISTING             3
#define OPEN_ALWAYS               4
#define TRUNCATE_EXISTING         5

#define FILE_SHARE_READ           0x00000001
#define FILE_SHARE_WRITE          0x00000002
#define OPEN_EXISTING             3
#define FILE_FLAG_OVERLAPPED      0x40000000
#define INVALID_FILE_SIZE         ((DWORD)0xFFFFFFFFl)
#define FILE_BEGIN                0
#define FILE_CURRENT              1
#define FILE_END                  2
#define ERROR_NO_SYSTEM_RESOURCES 1450L
#define ERROR_INVALID_USER_BUFFER 1784L
#define ERROR_NOT_ENOUGH_MEMORY   8L
#define ERROR_PATH_NOT_FOUND      3L
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
extern HANDLE CreateFile(
  const char* lpFileName,
  DWORD dwDesiredAccess,
  DWORD dwShareMode,
  void* lpSecurityAttributes,
  DWORD dwCreationDisposition,
  DWORD dwFlagsAndAttributes,
  HANDLE hTemplateFile
  );

//////////////////////////////////////////////////////////////////////////
extern DWORD GetFileAttributes(LPCSTR lpFileName);

//////////////////////////////////////////////////////////////////////////
extern BOOL SetFileAttributes(LPCSTR, DWORD attributes);

//////////////////////////////////////////////////////////////////////////
extern BOOL SetFileTime(
  HANDLE hFile,
  const FILETIME* lpCreationTime,
  const FILETIME* lpLastAccessTime,
  const FILETIME* lpLastWriteTime);
//////////////////////////////////////////////////////////////////////////
extern const uint64 GetFileModifTime(FILE* hFile);

//////////////////////////////////////////////////////////////////////////
extern DWORD GetFileSize(HANDLE hFile, DWORD* lpFileSizeHigh);

//////////////////////////////////////////////////////////////////////////
extern BOOL CloseHandle(HANDLE hObject);

//////////////////////////////////////////////////////////////////////////
extern BOOL    CancelIo(HANDLE hFile);
//////////////////////////////////////////////////////////////////////////
extern HRESULT GetOverlappedResult(HANDLE hFile, void* lpOverlapped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait);
//////////////////////////////////////////////////////////////////////////
extern BOOL    ReadFile(
  HANDLE hFile,
  LPVOID lpBuffer,
  DWORD nNumberOfBytesToRead,
  LPDWORD lpNumberOfBytesRead,
  LPOVERLAPPED lpOverlapped
  );

//////////////////////////////////////////////////////////////////////////
extern BOOL ReadFileEx(
  HANDLE hFile,
  LPVOID lpBuffer,
  DWORD nNumberOfBytesToRead,
  LPOVERLAPPED lpOverlapped,
  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
  );

//////////////////////////////////////////////////////////////////////////
extern DWORD SetFilePointer(
  HANDLE hFile,
  LONG lDistanceToMove,
  PLONG lpDistanceToMoveHigh,
  DWORD dwMoveMethod
  );

//////////////////////////////////////////////////////////////////////////
extern threadID GetCurrentThreadId();

//////////////////////////////////////////////////////////////////////////
extern HANDLE CreateEvent(
  LPSECURITY_ATTRIBUTES lpEventAttributes,
  BOOL bManualReset,
  BOOL bInitialState,
  LPCSTR lpName
  );

//////////////////////////////////////////////////////////////////////////
extern DWORD Sleep(DWORD dwMilliseconds);

//////////////////////////////////////////////////////////////////////////
extern DWORD SleepEx(DWORD dwMilliseconds, BOOL bAlertable);

//////////////////////////////////////////////////////////////////////////
extern DWORD WaitForSingleObjectEx(
  HANDLE hHandle,
  DWORD dwMilliseconds,
  BOOL bAlertable);

//////////////////////////////////////////////////////////////////////////
extern DWORD WaitForMultipleObjectsEx(
  DWORD nCount,
  const HANDLE* lpHandles,
  BOOL bWaitAll,
  DWORD dwMilliseconds,
  BOOL bAlertable);

//////////////////////////////////////////////////////////////////////////
extern DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

//////////////////////////////////////////////////////////////////////////
extern BOOL SetEvent(HANDLE hEvent);

//////////////////////////////////////////////////////////////////////////
extern BOOL ResetEvent(HANDLE hEvent);

//////////////////////////////////////////////////////////////////////////
extern HANDLE CreateMutex(
  LPSECURITY_ATTRIBUTES lpMutexAttributes,
  BOOL bInitialOwner,
  LPCSTR lpName
  );

//////////////////////////////////////////////////////////////////////////
extern BOOL ReleaseMutex(HANDLE hMutex);

//////////////////////////////////////////////////////////////////////////
typedef DWORD (*              PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

//////////////////////////////////////////////////////////////////////////
extern HANDLE CreateThread(
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  SIZE_T dwStackSize,
  LPTHREAD_START_ROUTINE lpStartAddress,
  LPVOID lpParameter,
  DWORD dwCreationFlags,
  LPDWORD lpThreadId
  );

extern BOOL  DeleteFile(LPCSTR lpFileName);
extern BOOL  MoveFile(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);
extern BOOL  RemoveDirectory(LPCSTR lpPathName);
extern DWORD GetCurrentDirectory(DWORD nBufferLength, char* lpBuffer);
//extern BOOL SetCurrentDirectory(LPCSTR lpPathName);
extern BOOL  CopyFile(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);
extern BOOL  GetComputerName(LPSTR lpBuffer, LPDWORD lpnSize);   //!< Required for CryOnline.
extern DWORD GetCurrentProcessId(void);

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus

//helper function
extern const bool GetFilenameNoCase(const char* file, char*, const bool cCreateNew = false);
extern void       adaptFilenameToLinux(char* rAdjustedFilename);
extern const int  comparePathNames(const char* cpFirst, const char* cpSecond, unsigned int len); //!< \return 0 if identical.
extern void       replaceDoublePathFilename(char* szFileName);                                   //!< Removes "\.\" to "\" and "/./" to "/".

//////////////////////////////////////////////////////////////////////////
extern char*    _fullpath(char* absPath, const char* relPath, size_t maxLength);
extern intptr_t _findfirst64(const char* filespec, struct __finddata64_t* fileinfo);
extern int      _findnext64(intptr_t handle, struct __finddata64_t* fileinfo);
extern int      _findclose(intptr_t handle);

//////////////////////////////////////////////////////////////////////////
extern void _makepath(char* path, const char* drive, const char* dir, const char* filename, const char* ext);
extern void _splitpath(const char* inpath, char* drv, char* dir, char* fname, char* ext);

//////////////////////////////////////////////////////////////////////////
extern int memicmp(LPCSTR s1, LPCSTR s2, DWORD len);
extern int strcmpi(const char* str1, const char* str2);

extern "C" char* strlwr(char* str);
extern "C" char* strupr(char* str);

extern char*     _ui64toa(unsigned long long value, char* str, int radix);
extern long long _atoi64(const char* str);

//////////////////////////////////////////////////////////////////////////
// function renaming
	#define _chmod    chmod
	#define _isnan    isnan
	#define stricmp   strcasecmp
	#define _stricmp  strcasecmp
	#define strnicmp  strncasecmp
	#define _strnicmp strncasecmp

	#define _strlwr   strlwr
	#define _strlwr_s(BUF, SIZE) strlwr(BUF)
	#define _strups   strupr

	#define _wtof(str) wcstod(str, 0)

int wcsicmp(const WCHAR* a, const WCHAR* b);
int wcsnicmp(const WCHAR* a, const WCHAR* b, size_t len);

	#define fprintf_s fprintf
	#define sscanf_s  sscanf
//////////////////////////////////////////////////////////////////////////

	#ifndef __TRLTOA__
		#define __TRLTOA__
extern char* ltoa(long i, char* a, int radix);
	#endif
	#define itoa ltoa

//////////////////////////////////////////////////////////////////////////
	#include <cmath>
using std::abs;
using std::sqrt;

extern char* _strtime(char* date);
extern char* _strdate(char* date);

#endif //__cplusplus

#define MemoryBarrier() _mm_mfence()

ILINE bool QueryPerformanceCounter(LARGE_INTEGER* counter)
{
	counter->QuadPart = __rdtsc();
	return true;
}

ILINE bool QueryPerformanceFrequency(LARGE_INTEGER* frequency)
{
	frequency->QuadPart = sceKernelGetTscFrequency();
	return true;
}

#define _flushall()
#define _strtoi64               strtoll

#define INVALID_HANDLE_VALUE    ((HANDLE)(LONG_PTR)-1)
#define INVALID_FILE_ATTRIBUTES (-1)

typedef int64 __time64_t;     /* 64-bit time value */

// Need to include this before it's used in finddata, but after the strnicmp definition
#include <CryString/CryString.h>

//! Attributes set by find request.
typedef struct __finddata64_t
{
	unsigned int attrib;            //!< Attributes, only directory and readonly flag actually set.
	__time64_t   time_create;       //!< Creation time, cannot parse under linux, last modification time is used instead (game does nowhere makes decision based on this values).
	__time64_t   time_access;       //!< Last access time.
	__time64_t   time_write;        //!< Last modification time.
	__time64_t   size;              //!< File size (for a directory it will be the block size).
	char         name[256];         //!< File/directory name.

private:
	int                 m_LastIndex;                 //!< Last index for findnext.
	char                m_DirectoryName[260];        //!< Directory name, needed when getting file attributes on the fly.
	char                m_ToMatch[260];              //!< Pattern to match with.
	void*               m_Dir;                       //!< Directory handle.
	std::vector<string> m_Entries;                   //!< All file entries in the current directories.
public:

	inline __finddata64_t() :
		attrib(0), time_create(0), time_access(0), time_write(0),
		size(0), m_LastIndex(-1), m_Dir(SCE_FIOS_DH_INVALID)
	{
		memset(name, '0', 256);
	}
	~__finddata64_t();

	//! Copies and retrieves the data for an actual match (to not waste any effort retrioeving data for unused files).
	void CopyFoundData(const char* rMatchedFileName);

public:
	//! Global _findfirst64 function using struct above, can't be a member function due to required semantic match.
	friend intptr_t _findfirst64(const char* pFileName, __finddata64_t* pFindData);

	//! Global _findnext64 function using struct above, can't be a member function due to required semantic match.
	friend int _findnext64(intptr_t last, __finddata64_t* pFindData);
}__finddata64_t;

//!< Need inheritance since in many places it get used as struct _finddata_t.
typedef struct _finddata_t : public __finddata64_t
{}_finddata_t;

extern int      _findnext64(intptr_t last, __finddata64_t* pFindData);
extern intptr_t _findfirst64(const char* pFileName, __finddata64_t* pFindData);

unsigned char CryInterlockedCompareExchange128(__int64 volatile*, __int64, __int64, __int64*);

const char* const ConvertFileName(char* const pBuf, const size_t bufSize, const char* const cpName);

// Enable file handle cache.
// This is not compatible with on-device shader compilation.
// Recommended to be disabled unless performance gain in final (shader-cached) builds is observed.
// #define USE_FILE_HANDLE_CACHE

#ifdef USE_FILE_HANDLE_CACHE
namespace FileIoWrapper {
enum FileAccessType
{
	GENERAL   = 0x0,
	DIRECT    = 0x40000000,
	STREAMING = 0x80000000
};
}

namespace std
{
FILE*  WrappedFopen(const char* _Restrict, const char* _Restrict, FileIoWrapper::FileAccessType type = FileIoWrapper::GENERAL, bool bSysAppHome = false);
int    WrappedStat(const char* _Filename, struct stat* _Stat);
int    WrappedFStat(int files, struct stat* _Stat);
int    WrappedFclose(FILE*);
size_t WrappedFSeek(FILE*, size_t, size_t);
size_t WrappedFtell(FILE*);
size_t WrappedFRead(void*, size_t, size_t, FILE*);
size_t WrappedFWrite(const void* ptr, size_t size, size_t count, FILE* stream);
size_t WrappedFileno(FILE*);
int    WrappedVFprintf(FILE* stream, const char* format, va_list arg);
char*  WrappedFGets(char* str, int num, FILE* stream);
int    WrappedFeof(FILE* stream);
int    WrappedFerror(FILE* stream);
int    WrappedGetc(FILE* stream);
int    WrappedUngetc(int character, FILE* stream);
int    WrappedFputs(const char* str, FILE* stream);
int    WrappedFflush(FILE* stream);
int    WrappedFprintf(FILE* stream, const char* format, ...);
int    WrappedFscanf(FILE* stream, const char* format, ...);
int    WrappedVFscanf(FILE* stream, const char* format, va_list arg);
}

using std::WrappedFopen;
using std::WrappedStat;
using std::WrappedFStat;
using std::WrappedFclose;
using std::WrappedFSeek;
using std::WrappedFtell;
using std::WrappedFRead;
using std::WrappedFWrite;
using std::WrappedFileno;
using std::WrappedVFprintf;
using std::WrappedFGets;
using std::WrappedFeof;
using std::WrappedFerror;
using std::WrappedGetc;
using std::WrappedUngetc;
using std::WrappedFputs;
using std::WrappedFflush;
using std::WrappedFprintf;
using std::WrappedFscanf;
using std::WrappedVFscanf;

	#undef fopen
	#define fopen WrappedFopen
	#undef stat
	#define stat(a, b) WrappedStat(a, b)
	#undef fstat
	#define fstat    WrappedFStat
	#undef fclose
	#define fclose   WrappedFclose
	#undef fseek
	#define fseek    WrappedFSeek
	#undef ftell
	#define ftell    WrappedFtell
	#undef fread
	#define fread    WrappedFRead
	#undef fwrite
	#define fwrite   WrappedFWrite
	#undef fileno
	#define fileno   WrappedFileno
	#undef vfprintf
	#define vfprintf WrappedVFprintf
	#undef fgets
	#define fgets    WrappedFGets
	#undef feof
	#define feof     WrappedFeof
	#undef ferror
	#define ferror   WrappedFerror
	#undef getc
	#define getc     WrappedGetc
	#undef ungetc
	#define ungetc   WrappedUngetc
	#undef fputs
	#define fputs    WrappedFputs
	#undef fflush
	#define fflush   WrappedFflush
	#undef fprintf
	#define fprintf  WrappedFprintf
	#undef fscanf
	#define fscanf   WrappedFscanf
	#undef vfscanf
	#define vfscanf  WrappedVFscanf
#endif // USE_FILE_HANDLE_CACHE

#endif // __Orbis_Win32Wrapper_h__
