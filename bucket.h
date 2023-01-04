#pragma once
#include <cstdint>
#include <cstddef>
#include <iostream>

// sustituir por los asserts del engine 
// al que se va a hacer el traspaso
#include <assert.h>

class Bucket
{
public:

	Bucket(size_t block_size, size_t block_count);
	~Bucket();

	// Tests if the pointer belongs to this bucket
	bool belongs(void* ptr) const noexcept;

	// Returns nullptr if failed
	[[nodiscard]] void* allocate(size_t bytes) noexcept;
	void deallocate(void* ptr, size_t bytes)   noexcept;

public:
	const size_t _blockSize{};
	const size_t _blockCount{};

private:
	// Finds n free contiguous blocks in the ledger and returns the first block´s index or BlockCounr on failer
	size_t findContiguousBlocks(size_t n) const noexcept;

	// Marks n blocks in the ledger as "in-use" starting at 'index'
	void setBlocksInUse(size_t idx, size_t n)   noexcept;

	// Marks n blocks in the ledger as  "free"  starting at 'index'
	void setBlocksFree(size_t idx, size_t n)    noexcept;

	// Actual memory for allocations
	std::byte* m_data   { nullptr };
	// Reserves one bit per block to indicate wether it is in-use
	std::byte* m_ledger { nullptr };
};