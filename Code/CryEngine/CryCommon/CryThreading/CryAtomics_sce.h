// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <sce_atomic.h>
#include <CryCore/Assert/CryAssert.h>

//////////////////////////////////////////////////////////////////////////
// Interlocked API
//////////////////////////////////////////////////////////////////////////

// Returns the resulting incremented value
ILINE LONG CryInterlockedIncrement(volatile LONG* pDst)
{
	static_assert(sizeof(int) == sizeof(int32_t), "Unsecured cast. int is not same size as int32_t.");
	return sceAtomicIncrement32((volatile int32_t*)pDst) + 1;
}

// Returns the resulting decremented value
ILINE LONG CryInterlockedDecrement(volatile LONG* pDst)
{
	static_assert(sizeof(int) == sizeof(int32_t), "Unsecured cast. int is not same size as int32_t.");
	return sceAtomicDecrement32((volatile int32_t*)pDst) - 1;
}

// Returns the resulting added value
ILINE LONG CryInterlockedAdd(volatile LONG* pDst, LONG add)
{
	static_assert(sizeof(LONG) == sizeof(int32_t), "Unsecured cast. LONG is not same size as int32_t.");
	return sceAtomicAdd32((volatile int32_t*)pDst, add) + add;
}

// Returns the resulting added value
ILINE size_t CryInterlockedAdd(volatile size_t* pDst, size_t add)
{
#ifdef CRY_PLATFORM_64BIT
	static_assert(sizeof(size_t) == sizeof(int64_t), "Unsecured cast. LONG is not same size as int64_t.");
	return sceAtomicAdd64((volatile int64_t*)pDst, add) + add;
#else
	static_assert(sizeof(size_t) == sizeof(int32_t), "Unsecured cast. LONG is not same size as int32_t.");
	return CryInterlockedAdd((volatile int32_t*)pDst, add) + add;
#endif
}

// Returns initial value prior exchange
ILINE LONG CryInterlockedExchange(volatile int* pDst, int exchange)
{
	static_assert(sizeof(LONG) == sizeof(int32_t), "Unsecured cast. LONG is not same size as int32_t.");
	return sceAtomicExchange32((volatile int32_t*)pDst, (int32_t)exchange);
}

// Returns initial value prior exchange
ILINE int64 CryInterlockedExchange64(volatile int64_t* addr, int64_t exchange)
{
	return sceAtomicExchange64(addr, exchange);
}

// Returns initial value prior exchange
ILINE LONG CryInterlockedExchangeAdd(volatile LONG* pDst, LONG value)
{
	static_assert(sizeof(LONG) == sizeof(int32_t), "Unsecured cast. LONG is not same size as int32_t.");
	return sceAtomicAdd32((volatile int32_t*)pDst, (int32_t)value);
}

// Returns initial value prior exchange
ILINE size_t CryInterlockedExchangeAdd(volatile size_t* pDst, size_t value)
{
#ifdef CRY_PLATFORM_64BIT
	static_assert(sizeof(size_t) == sizeof(int64_t), "Unsecured cast. size_t is not same size as int64_t.");
	return sceAtomicAdd64((volatile int64_t*)pDst, (int64_t)value);
#else
	static_assert(sizeof(size_t) == sizeof(int32_t), "Unsecured cast. size_t is not same size as int32_t.");
	return CryInterlockedAdd((volatile int32_t*)pDst, (int32_t)value);
#endif
}

// Returns initial value prior exchange
ILINE LONG CryInterlockedExchangeAnd(volatile LONG* pDst, LONG value)
{
	static_assert(sizeof(LONG) == sizeof(int32_t), "Unsecured cast. LONG is not same size as int32_t.");
	return sceAtomicAnd32((volatile int32_t*)pDst, (int32_t)value);
}

// Returns initial value prior exchange
ILINE LONG CryInterlockedExchangeOr(volatile LONG* pDst, LONG value)
{
	static_assert(sizeof(LONG) == sizeof(int32_t), "Unsecured cast. LONG is not same size as int32_t.");
	return sceAtomicOr32((volatile int32_t*)pDst, (int32_t)value);
}

// Returns initial address prior exchange
ILINE void* CryInterlockedExchangePointer(void* volatile* pDst, void* exchange)
{
#if CRY_PLATFORM_64BIT
	static_assert(sizeof(void*) == sizeof(int64_t), "Unsecured cast. void* is not same size as int64_t.");
	return (void*)sceAtomicExchange64((volatile int64_t*)pDst, (int64_t)exchange);
#else
	return (void*)CryInterlockedExchange((volatile LONG*)pDst, (LONG)exchange);
#endif
}

// Returns initial value prior exchange
ILINE LONG CryInterlockedCompareExchange(LONG volatile* pDst, LONG exchange, LONG comperand)
{
	static_assert(sizeof(LONG) == sizeof(int32_t), "Unsecured cast. LONG is not same size as int32_t.");
	return sceAtomicCompareAndSwap32((volatile int32_t*)pDst, (int32_t)comperand, (int32_t)exchange);
}

// Returns initial value prior exchange
ILINE int64 CryInterlockedCompareExchange64(volatile int64* addr, int64 exchange, int64 compare)
{
	static_assert(sizeof(int64) == sizeof(int64_t), "Unsecured cast. int64 is not same size as int64_t.");
	return sceAtomicCompareAndSwap64((volatile int64_t*)addr, (int64_t)compare, (int64_t)exchange);
}

#if CRY_PLATFORM_64BIT
// Returns initial value prior exchange
ILINE unsigned char CryInterlockedCompareExchange128(volatile int64* pDst, int64 exchangehigh, int64 exchangelow, int64* pComparandResult)
{
	static_assert(sizeof(int64) == sizeof(int64_t), "Unsecured cast. int64 is not same size as int64_t.");
	//CRY_ASSERT_MESSAGE(((int64)pDst) & 15) == 0, "The destination data must be 16-byte aligned to avoid a general protection fault.");

	#if LITTLE_ENDIAN
	return sceAtomicCompareAndSwap128_64((int64_t*)pDst, (int64_t)pComparandResult[1], (int64_t)pComparandResult[0], (int64_t)exchangehigh, (int64_t)exchangelow, (int64_t*)pComparandResult);
	#else
	return sceAtomicCompareAndSwap128_64((int64_t*)pDst, (int64_t)pComparandResult[0], (int64_t)pComparandResult[1], (int64_t)exchangehigh, (int64_t)exchangelow, (int64_t*)pComparandResult);
	#endif
}
#endif

// Returns initial address prior exchange
ILINE void* CryInterlockedCompareExchangePointer(void* volatile* pDst, void* exchange, void* comperand)
{
#if CRY_PLATFORM_64BIT
	static_assert(sizeof(void*) == sizeof(int64_t), "Unsecured cast. void* is not same size as int64_t.");
	return (void*)sceAtomicCompareAndSwap64((volatile int64_t*)pDst, (int64_t)comperand, (int64_t)exchange);
#else
	static_assert(sizeof(void*) == sizeof(LONG), "Unsecured cast. void* is not same size as LONG.");
	return (void*)CryInterlockedCompareExchange((volatile LONG*)pDst, (LONG)exchange, (LONG)comperand);
#endif
}
//////////////////////////////////////////////////////////////////////////
// Implementation for ORBIS using pointer-embedded salt
// This works because we know the maximum virtual address that can be used
//////////////////////////////////////////////////////////////////////////
struct SSalt
{
	// Find log2 of some address, rounded down
	// Note: Pass in X + X - 1 to round up instead
	template<uintptr_t X, uintptr_t R = 0>
	struct log2 : log2<X / 2, R + 1> {};
	template<uintptr_t R>
	struct log2<1, R> { static const uintptr_t value = R; };

	// Analyze memory range to find maximum possible salt values
	static const uintptr_t kMemStart = SCE_KERNEL_APP_MAP_AREA_START_ADDR;
	static const uintptr_t kMemEnd = SCE_KERNEL_APP_MAP_AREA_END_ADDR;
	static_assert(kMemStart < kMemEnd, "invalid memory range");
	static const uintptr_t kSaltOne = uintptr_t(1) << log2<kMemEnd + kMemEnd - 1>::value;
	static const uintptr_t kPointerMask = kSaltOne - 1;
	static const uintptr_t kSaltMask = ~kPointerMask;
};

//////////////////////////////////////////////////////////////////////////
ILINE void CryInterlockedPushEntrySList(SLockFreeSingleLinkedListHeader& list, SLockFreeSingleLinkedListEntry& element)
{
	uintptr_t head;
	uintptr_t newHead;
	do
	{
		head = (volatile uintptr_t)list.pNext;
		newHead = (head& SSalt::kSaltMask) +SSalt::kSaltOne + (uintptr_t)&element;
		element.pNext = (SLockFreeSingleLinkedListEntry*)(head & SSalt::kPointerMask);
	}
	while (sceAtomicCompareAndSwap64((volatile int64_t*)&list.pNext, (int64_t)head, (int64_t)newHead) != head);
}

//////////////////////////////////////////////////////////////////////////
ILINE void CryInterlockedPushListSList(SLockFreeSingleLinkedListHeader& list, SLockFreeSingleLinkedListEntry& first, SLockFreeSingleLinkedListEntry& last, uint32 count)
{
	(void)count; // not used

	uintptr_t head;
	uintptr_t newHead;
	do
	{
		head = (volatile uintptr_t)list.pNext;
		newHead = (head& SSalt::kSaltMask) +SSalt::kSaltOne + (uintptr_t)&first;
		last.pNext = (SLockFreeSingleLinkedListEntry*)(head & SSalt::kPointerMask);
	}
	while (sceAtomicCompareAndSwap64((volatile int64_t*)&list.pNext, (int64_t)head, (int64_t)newHead) != head);
}

//////////////////////////////////////////////////////////////////////////
ILINE void* CryInterlockedPopEntrySList(SLockFreeSingleLinkedListHeader& list)
{
	uintptr_t head;
	uintptr_t newHead;
	SLockFreeSingleLinkedListEntry* pResult;
	do
	{
		head = (volatile uintptr_t)list.pNext;
		pResult = (SLockFreeSingleLinkedListEntry*)(head & SSalt::kPointerMask);
		if (!pResult) break;
		newHead = (head& SSalt::kSaltMask) +SSalt::kSaltOne + (uintptr_t)pResult->pNext;
	}
	while (sceAtomicCompareAndSwap64((volatile int64_t*)&list, (int64_t)head, (int64_t)newHead) != head);
	return pResult;
}

//////////////////////////////////////////////////////////////////////////
ILINE void* CryRtlFirstEntrySList(SLockFreeSingleLinkedListHeader& list)
{
	uintptr_t head = sceAtomicLoad64((volatile int64_t*)&list.pNext);
	return (SLockFreeSingleLinkedListEntry*)(head & SSalt::kPointerMask);
}

//////////////////////////////////////////////////////////////////////////
ILINE void CryInitializeSListHead(SLockFreeSingleLinkedListHeader& list)
{
	list.pNext = nullptr;
}

//////////////////////////////////////////////////////////////////////////
ILINE void* CryInterlockedFlushSList(SLockFreeSingleLinkedListHeader& list)
{
	uintptr_t head;
	uintptr_t newHead;
	SLockFreeSingleLinkedListEntry* pResult;
	do
	{
		head = (volatile uintptr_t)list.pNext;
		pResult = (SLockFreeSingleLinkedListEntry*)(head & SSalt::kPointerMask);
		if (!pResult) break;
		newHead = (head& SSalt::kSaltMask) +SSalt::kSaltOne;
	}
	while (sceAtomicCompareAndSwap64((volatile int64_t*)&list, (int64_t)head, (int64_t)newHead) != head);
	return pResult;
}

//////////////////////////////////////////////////////////////////////////
// Helper
//////////////////////////////////////////////////////////////////////////
class CSimpleThreadBackOff
{
public:
	static const uint32 kSoftYieldInterval = 0x3FF;
	static const uint32 kHardYieldInterval = 0x1FFF;

public:
	CSimpleThreadBackOff() : m_counter(0) {}

	void reset() { m_counter = 0; }

	void backoff()
	{
		_mm_pause();

		if (!(++m_counter & kHardYieldInterval))
		{
			sceKernelUsleep(1); // give threads with other prio chance to run
		}
		else if (!(m_counter & kSoftYieldInterval))
		{
			scePthreadYield(); // give threads with same prio chance to run
		}
	}

private:
	int m_counter;
};
