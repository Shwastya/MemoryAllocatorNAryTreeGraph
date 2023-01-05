/*
* https://www.youtube.com/watch?v=l14Zkx5OXr4&ab_channel=CppCon
------------------------- Memory Pool implementation -------------------------

- Una implementaci�n m�nima es suficiente como ejemplo:

	* Algunas comprobaciones de errores se omiten para una mejor legibilidad
	* La implementacion no es thread-safe
	* Se asume la alineaci�n predeterminada

- Sin embargo, se pueden definir varias instancias en la misma aplicaci�n.

	* Las aplicaciones cr�ticas de seguridad generalmente requieren la
	  separaci�n del grupo de memoria
	* Un grupo por subproceso permitir�a evitar el bloqueo por completo
*/
#pragma once
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <assert.h> // sustituir por los asserts del engine

class bucket
{
public:

	bucket(size_t block_size, size_t block_count);
	~bucket();

	// Tests if the pointer belongs to this bucket
	bool belongs(void* ptr) const noexcept;

	// Returns nullptr if failed
	[[nodiscard]] void* allocate(size_t bytes) noexcept;
	void deallocate(void* ptr, size_t bytes)   noexcept;

public:
	const size_t _blockSize{};
	const size_t _blockCount{};

private:
	// Finds n free contiguous blocks in the ledger and returns the first block�s index or BlockCounr on failer
	size_t findContiguousBlocks(size_t n) const noexcept;

	// Marks n blocks in the ledger as "in-use" starting at 'index'
	void setBlocksInUse(size_t idx, size_t n)   noexcept;

	// Marks n blocks in the ledger as  "free"  starting at 'index'
	void setBlocksFree(size_t idx, size_t n)    noexcept;

	// Actual memory for allocations
	std::byte* m_data{ nullptr };
	// Reserves one bit per block to indicate wether it is in-use
	std::byte* m_ledger{ nullptr };
};

/*
------------------------- How to configure the pools? -------------------------

- Una instancia de memory pool es simplemente una colecci�n de buckets
- Cada bucket tiene 2 propiedades BlockSize y BlockCount
- Una colecci�n de estas propiedades por bucket definir� una instancia de pool


	- �C�mo ser�a una colecci�n de par�metros?
	- �C�mo definir�amos m�s de una instancia de grupo?

*/

// 1. La implementacion por defecto define a 'pool' sin 'buckets'

template<size_t id>
struct bucket_descriptors
{
	// empty tuple says: this memory pool its not defined
	// we dont have any buckets for this, so, if you instantiate it
	// with any ID you get like empty memory pool

	using type = std::tuple<>;
};


// 2. Pero, que podemos hacer si queremos especializar esto para ciertos IDs
// y definir 'memory pools' de esa forma

struct bucket_cfg16 {
	static constexpr size_t BlockSize = 16;
	static constexpr size_t BlockCount = 10000;
};
struct bucket_cfg32 {
	static constexpr size_t BlockSize = 32;
	static constexpr size_t BlockCount = 10000;
};
struct bucket_cfg1024 {
	static constexpr size_t BlockSize = 1024;
	static constexpr size_t BlockCount = 50000;
};
template<>
struct bucket_descriptors<1>
{
	// specializaciones para 'bucket_descriptors' de 'memory pool' uno <1> 
	// el cual contiene 3 'buckets' 

	using type = std::tuple<bucket_cfg16, bucket_cfg32, bucket_cfg1024>;
};


/*  Defining some 'boiler-plates' to sabe some space on next slides */

// pero para hacer esto convenientemente, cuando cogemos esto como una idea
// de memory pool y el indice del bucket dentro del tuple, y cogemos BlockSize o BlockCount como valor 

template<size_t id>
using bucket_descriptors_t = typename bucket_descriptors<id>::type;

template<size_t id>
static constexpr size_t bucket_count = std::tuple_size<bucket_descriptors_t<id>>::value;

template<size_t id>
using pool_type = std::array<bucket, bucket_count<id>>;

template<size_t id, size_t Idx>
struct get_size : std::integral_constant<size_t, std::tuple_element_t<Idx, bucket_descriptors_t<id>>::BlockSize> {};
template<size_t id, size_t Idx>
struct get_count : std::integral_constant<size_t, std::tuple_element_t<Idx, bucket_descriptors_t<id>>::BlocKCount> {};


// con todo esto podemos definir nuestros pools facilmente
// podemos definirlos con funcion get_instances la cual es
// instanciada con un memory pool ID y esto definira un static local al array (pool type es solo el array)
// esto llamara get_size y get_count en cada bucket desde la izq. a la der. e instanciara con nuestro
// array estos buckets por tanto CUANDO LLAMAMOS get_instance LA PRIMERA VEZ, cada cosa es allocated 
// y creada en nuestra memory pool con este ID 

template<size_t id, size_t... Idx>
auto& get_instance(std::index_sequence<Idx...>) noexcept
{
	static pool_type<id> instance{ {{get_size<id, Idx>::value, get_count<id, Idx>::value} ...} };
	return instance;
}

template<size_t id>
auto& get_instance() noexcept
{
	return get_instance<id>(std::make_index_sequence<bucket_count<id>>());
}

/* How to choose the right bucket ???? */

// �Qu� tal simplemente revisar el cubo hasta encontrar uno lo suficientemente grande?
//			 (Suponiendo que est�n ordenados por tama�o de bloque)

// asumiendo buckets son ordenados por tama�o de bloques

// ESTRATEGIA: ingenua de "Primer Ajuste" <<First Fit>>
#if 0
template<size_t id>
[[nodiscard]] void* allocate(size_t bytes)
{
	auto& pool = get_instance<id>();

	for (auto& bucket : pool)
	{
		if (bucket.BlockSize >= bytes)
		{
			if (auto ptr = bucket.allocate(bytes); ptr != nullptr)
			{
				return ptr;
			}
		}
	}
	throw std::bad_alloc{};
}
#endif
/*
*
* �Como escoger el bucket adecuado?
*
* Esto funcionar� para los tipos que siempre asignan memoria para sus objetos
* uno por uno (como std::map), pero no para aquellos que asignan memoria para
* multiples objetos a la vez (como std::unordered_map)
*
*	- Puede que simplemente no haya un bucket lo suficientemente grande
*   - Un bucket "equivocado" se puede elegir, el cu�l ser� "robado" de otro tipo
*
*     Ya que unorderer map aloja dos cosas distintas de dos maneras diferentes
*
*								has_node<int, string>
*                              /
*	unorderer_map<int, string>
*							   \
*								has_node<int, string>*
*
*/


// ESTRATEGIA: "Mas cercano al real"

/*
* Cuando se asigna desde un bucket, se desconoce si la asignaci�n es para uno
* o varios objetos; solo se proporciona el tama�o en bytes, es para uno o
* varios objetos; solo se proporciona el tama�o en bytes
*
* NOTA: Esto tambi�n imita c�mo std::pmr::memory_resource define la interfaz de asignaci�n
*
* La �nica forma de resolver este problema ser�a encontrar qu� asignaci�n conducir�a
* a la menor cantidad de memoria desperdiciada y tomar�a la menor cantidad de bloques.
*/
#if 0
struct info
{
	size_t index{ 0 };		 // que bucket?
	size_t block_count{ 0 }; // cuantos bloques se desean alojar del bucket
	size_t waste{ 0 };		 // Cuanta memoria se va a utilizar?

	bool operator<(const info& other) const noexcept
	{
		return (waste == other.waste) ? block_count < other.block_count : waste < other.waste;
	}
};
template<size_t id>
[[nodsicard]] void* allocate(size_t bytes)
{
	auto& pool = get_instance<id>();

	std::array <info, bucket_count<id>> deltas;
	std::size_t index = 0;

	for (const auto& bucket : pool)
	{
		deltas[index].index = index;

		if (bucket.BlockSize >= bytes)
		{
			deltas[index].waste = bucket.BlockSize - bytes;
			deltas[index].block_count = 1;
		}
		else
		{
			const auto n = 1 + ((bytes - 1) / bucket.BlockSize);
			const auto storage_required = n * bucket.BlockSize;
			deltas[index].waste = storage_required - bytes;
			deltas[index].block_count = n;
		}
		++index;
	}

	sort(deltas.begin(), deltas.end()); // sort() is permitido to allocate

	for (const auto& d : deltas)
	{
		if (auto ptr = pool[d.index].allocate(bytes); ptr != nullptr)
			return ptr;
	}
	throw std::bad_alloc{};
}
#endif
/*
* esta no es la mejor solucion, es solo una solucion simple
* mejor solucion seria hacer un mapa de tipos directamente a los buckets
* hashing por Tag names o algo similar
*/

/*
* Con lo anterior tenemos un problema tambien:
*
* Introducimos algun tipo de fragmentacion aqui ya que
* introducimos multiples bloques, eso signfica que es probable
* que se introduzcan con agujeros entre espacios de memoria
* no es una soluci�n 100% segura para evitar la fragmentacion
*
* Por eso para algunos casos puede ser conveniente usar
* la solucion simple
*/

// la siguiente funcion permite escoger entre las dos estrategias
// con un parametro de tipo bool

struct info
{
	size_t index{ 0 };		 // que bucket?
	size_t block_count{ 0 }; // cuantos bloques se desean alojar del bucket
	size_t waste{ 0 };		 // Cuanta memoria se va a utilizar?

	bool operator<(const info& other) const noexcept
	{
		return (waste == other.waste) ? block_count < other.block_count : waste < other.waste;
	}
};
template<size_t id>
[[nodsicard]] void* allocate(size_t bytes, bool simple_solution = true)
{
	auto& pool = get_instance<id>();


	// simple solution "Primer Ajuste" <<First Fit>>
	if (simple_solution)
	{
		for (auto& bucket : pool)
		{
			if (bucket.BlockSize >= bytes)
			{
				if (auto ptr = bucket.allocate(bytes); ptr != nullptr)
				{
					return ptr;
				}
			}
		}
	}
	// second solution "Mas cercano al real"
	else
	{
		std::array <info, bucket_count<id>> deltas;
		std::size_t index = 0;

		for (const auto& bucket : pool)
		{
			deltas[index].index = index;

			if (bucket.BlockSize >= bytes)
			{
				deltas[index].waste = bucket.BlockSize - bytes;
				deltas[index].block_count = 1;
			}
			else
			{
				const auto n = 1 + ((bytes - 1) / bucket.BlockSize);
				const auto storage_required = n * bucket.BlockSize;
				deltas[index].waste = storage_required - bytes;
				deltas[index].block_count = n;
			}
			++index;
		}

		sort(deltas.begin(), deltas.end()); // sort() is permitido to allocate

		for (const auto& d : deltas)
		{
			if (auto ptr = pool[d.index].allocate(bytes); ptr != nullptr)
				return ptr;
		}
	}
	throw std::bad_alloc{};
}

/* deallocate */ // very symple
template<size_t id>
void deallocate(void* ptr, size_t bytes) noexcept
{
	auto& pool = get_instance<id>();

	for (auto& bucket : pool)
	{
		if (bucket.belongs(ptr))
		{
			bucket.deallocate(ptr, bytes);
			return;
		}
	}
}
// seguimos necesitando definir dos funciones 
// para el uso del allocator
// hemos definido implementacions por defecto de nuestros buckets descriptors
// que actualmente es un tuple 0 (que significa que el memory pool no esta definido)
// por tanto tenemos estas funciones de inicializacion
template<size_t id>
constexpr bool is_defined() noexcept
{
	return bucket_count<id> != 0;
}

template<size_t id>
bool initialize() noexcept
{
	(void)get_instance<id>();
	return is_defined<id>();
}





