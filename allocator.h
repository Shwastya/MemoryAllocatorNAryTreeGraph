#pragma once

template<typename T>
class allocator
{
	// ...

	template<typename U>
	struct rebind
	{
		using other = allocator<U>;
	};

	// ...
	using pointer = T*;
	// ...

	pointer allocate(size_type n, const void* hint = 0)
	{
		// has to allocate n * sizeof(T) bytes
	}
	// ...
};