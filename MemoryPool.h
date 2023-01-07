#pragma once
#include <iostream>
#include <utility>
#include <iomanip>
#include <list>
#include <new>
#include <atomic>
#include <mutex>
#include <cstddef>
#include <cstdlib>

template<size_t chunk_size, size_t chunks_per_block>
class Pool
{
public:
	static_assert(chunk_size >= sizeof(void*));
	static_assert(chunks_per_block > 0);

	explicit Pool() = default;

	Pool(const Pool&) = delete;
	Pool(Pool&& other)
	{}

	Pool& operator = (Pool other)
	{
		return *this;
	}
	~Pool()	{ for (auto ptr : m_blocks) std::free(ptr);	}

	void* malloc()
	{
		if (m_next_chunk == nullptr) m_next_chunk =  allocate_block(nullptr);
		chunk_t* chunk = m_next_chunk;
		m_next_chunk   = m_next_chunk->m_next;
		std::cout << "Malloc chunk #" << std::setw(2) << std::setfill('0') << chunk->m_sequence << " " << chunk << /*" <- " << chunk->Tag <<*/ std::endl;
		return chunk;
	}

	void free(void* ptr)
	{
		chunk_t* chunk = reinterpret_cast<chunk_t*>(ptr);
		chunk->m_next  = m_next_chunk;
		m_next_chunk   = chunk;
		std::cout << "Free   chunk #" << std::setw(2) << std::setfill('0') << chunk->m_sequence << " " << chunk << /*" <- " << chunk->Tag <<*/ std::endl;
	}

	void reserveBlocks(size_t blocks)
	{
		while (blocks > m_blocks.size())
		{
			m_next_chunk = allocate_block(m_next_chunk);
		}
	}

private:
	struct chunk_t
	{
		chunk_t(chunk_t* next = nullptr) : m_next { next } // next address
		{
			std::cout << "Create chunk #" << std::setw(2) << std::setfill('0') << m_sequence << " " << this << " -> " << m_next << std::endl;
		}
		union
		{
			chunk_t*  m_next;
			std::byte m_storage[chunk_size];			
		};


		size_t m_sequence = ++k_sequence;
		inline static size_t k_sequence = 0;
	};

private:
	chunk_t* allocate_block(chunk_t* tail)
	{
		std::unique_lock guard(m_lock);
		if (m_next_chunk != nullptr) return m_next_chunk;

		chunk_t* block
		{
			reinterpret_cast<chunk_t*>(std::malloc(sizeof(chunk_t) * chunks_per_block))
		};

		if (block == nullptr) throw std::bad_alloc();

		m_blocks.push_back(block);

		chunk_t* chunk = block;

		for (size_t i = 0; i < chunks_per_block - 1; ++i)
		{
			new (chunk) chunk_t { chunk + 1 };
			chunk = chunk->m_next;
		}
		new (chunk) chunk_t { tail };

		return block;
	}

	chunk_t* m_next_chunk { nullptr };

	std::mutex m_lock;
	using block_list = std::list<void*>;
	block_list m_blocks;
};

