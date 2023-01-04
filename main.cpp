#include "SceneGraph.h"
#include "Node.h"
#include "debugTools.h"
int main()
{
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
	return 0;
}