
#include "SceneGraph.h"
#include "Node.h"
#include "debugTools.h"

#include "MemoryPool.h"
#include <random>

struct NodeObject
{
	uint64_t X{};
	uint64_t y{};
};

int main()
{
#if 0
	{
		SceneGraph SG;

		SG.createRootChild()->setTag("ROOT->Child[0]");
		SG.createRootChild()->setTag("ROOT->Child[1]");


		SG.getRoot()->childByTag("ROOT->Child[0]")->createChild()->setTag("ROOT->Child[0]->Child[0]");
		SG.getRoot()->childByTag("ROOT->Child[0]")->createChild()->setTag("ROOT->Child[0]->Child[1]");
		SG.getRoot()->childByTag("ROOT->Child[0]")->createChild()->setTag("ROOT->Child[0]->Child[2]");

		SG.getRoot()->childByTag("ROOT->Child[1]")->createChild()->setTag("ROOT->Child[1]->Child[0]");
		SG.getRoot()->childByTag("ROOT->Child[1]")->createChild()->setTag("ROOT->Child[1]->Child[1]");
		SG.getRoot()->childByTag("ROOT->Child[1]")->createChild()->setTag("ROOT->Child[1]->Child[2]");

		SG.getRoot()->childByTag("ROOT->Child[1]")->childByTag("ROOT->Child[1]->Child[0]")->createChild()->setTag("ROOT->Child[1]->Child[0]->Child[0]");
		SG.getRoot()->childByTag("ROOT->Child[1]")->childByTag("ROOT->Child[1]->Child[0]")->createChild()->setTag("ROOT->Child[1]->Child[0]->Child[1]");

		SG.update();

		const Node* n = SG.getRoot()->BFS("ROOT->Child[1]->Child[0]");
		if (n) std::cout << "--> " << n->getTag() << std::endl;
	}
	MemoryUsage();

#endif
	
	{
		constexpr auto Chunks = 16;

		Pool<sizeof(void*), 16> pool;

		std::vector<void*> chunks;

		//pool.reserveBlocks(3);


		for (int i = 0; i < Chunks; ++i) 
		{
			void* chunk = pool.malloc();
			chunks.push_back(chunk);
		}

		//std::random_device dev;
		//std::mt19937 gen { dev() };

		//std::shuffle(chunks.begin(), chunks.end(), gen);

		for (auto c : chunks) pool.free(c);
	}
	MemoryUsage();
	return 0;
}