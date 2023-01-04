#pragma once
#include <cinttypes>
#include <crtdbg.h>
#include <iostream>

#if 1 && _DEBUG

struct MemoryTracking
{
	uint32_t TotalAllocated{ 0 };
	uint32_t TotalFreed{ 0 };
	uint32_t TotalAllocs{ 0 };
};

static MemoryTracking s_metrics;

#pragma warning( push )
#pragma warning( disable : 28196 )
#pragma warning( disable : 28251 )
#pragma warning( disable : 6387 )
void* operator new(const size_t size)
{
	s_metrics.TotalAllocated += static_cast<uint32_t>(size);
	++s_metrics.TotalAllocs;
#if 0
	printf("  |-> Allocating %" PRIuPTR " bytes\n", size);
#endif
	return malloc(size);	// breakpoint here for allocates tracking
}
#pragma warning( pop )

void operator delete(void* memory, const size_t size)
{
	s_metrics.TotalFreed += static_cast<uint32_t>(size);
#if 0
	printf("  |-> Freeing %" PRIuPTR " bytes\n", size);
#endif
	free(memory);			// breakpoint here for freeds tracking
}

static void printfMemoryUsage()
{
	const uint32_t totalFreed{ (s_metrics.TotalFreed) };
	printf("\n    Total allocated: %u bytes --> Total allocs: %u\n", s_metrics.TotalAllocated, s_metrics.TotalAllocs);
	printf("    Total freeds:    %u bytes\n", totalFreed);
	printf("    Memory leaks:    %u bytes\n", s_metrics.TotalAllocated - totalFreed);

	if ((s_metrics.TotalAllocated - totalFreed) > 0) _CrtDumpMemoryLeaks();
}

#if 1 
#define MemoryUsage() printfMemoryUsage()
#endif
#else
#define MemoryUsage() (void)0
#endif // DEBUG
