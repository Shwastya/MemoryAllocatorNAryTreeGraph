#include "bucket.h"

bucket::bucket(size_t block_size, size_t block_count)
	: _blockSize(block_size)
	, _blockCount(block_count)
{
	const auto data_size = _blockSize * _blockCount;
	m_data = static_cast<std::byte*>(std::malloc(data_size));
	assert(m_data != nullptr);

	const auto ledger_size = 1 + ((_blockCount - 1) / 8);
	m_ledger = static_cast<std::byte*>(std::malloc(ledger_size));
	assert(m_ledger != nullptr);

	memset(m_data, 0, data_size);
	memset(m_ledger, 0, ledger_size);
}

bucket::~bucket()
{
	std::free(m_ledger);
	std::free(m_data);
}

bool bucket::belongs(void* ptr) const noexcept
{
	return false;
}

void* bucket::allocate(size_t bytes) noexcept
{
	// Calculate the required number of blocks
	const auto n = 1 + ((bytes - 1) / _blockSize);

	const auto idx = findContiguousBlocks(n);

	if (idx == _blockCount)	return nullptr;

	setBlocksInUse(idx, n);

	return m_data + (idx * _blockSize);
}

void bucket::deallocate(void* ptr, size_t bytes) noexcept
{
	const auto p = static_cast<const std::byte*>(ptr);
	const size_t dist = static_cast<size_t>(p - m_data);

	// Calculate blovk index from pointer distance
	const auto idx = dist / _blockSize;

	// Calculate the required number of blocks
	const auto n = 1 + ((bytes - 1) / _blockSize);

	// Update the ledger
	setBlocksFree(idx, n);
}

size_t bucket::findContiguousBlocks(size_t n) const noexcept
{
	return size_t();
}

void bucket::setBlocksInUse(size_t idx, size_t n) noexcept
{
}

void bucket::setBlocksFree(size_t idx, size_t n) noexcept
{
}
