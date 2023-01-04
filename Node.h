#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <queue>

class SceneGraph;
class Node
{
public:
	struct Layer 
	{
		enum Enum : uint8_t
		{
			Enabled   = 1 << 0,
			Recursive = 1 << 1,
			Selected  = 1 << 2,
			Default	  = Enabled
		};		
	};

	Node();
	~Node();	

	Node(const Node&) = default;
	Node& operator=(const Node&) = default;

	Node(Node&&) noexcept = default;
	Node& operator=(Node&&) noexcept = default;

	void enable();
	void disable();
	[[nodiscard]] bool isEnabled() const;

	void enableRecursive();
	void disableRecursive();
	[[nodiscard]] bool isEnabledRecursive() const;	

	void setTag(const char* tag);
	[[nodiscard]] const std::string& getTag() const;

	[[nodiscard]] uint32_t getDepth() const;

	Node* getParent();
	[[nodiscard]] const Node* getParent() const;

	Node* createChild();
	//common_code::Memory<Node* []> createChildren(uint32_t number);

	[[nodiscard]] const Node* childAt(size_t idx) const;
	Node* childAt(size_t idx);
	Node* childByTag(const char* tag, bool recursive = false, Node* _node = nullptr);	

	[[nodiscard]] size_t numChildren() const;
	uint32_t getDepth();

	const std::string& getEntity();

	// Recursive Children visit Debug
	void debugVisit(Node* _node = nullptr) const {
		size_t idx = 0;
		for (;;) {
			const Node* node = childAt(idx++);			
			if (!node) break;
			// debug uncomment
			std::cout << node->getTag() << std::endl;
			node->debugVisit(_node);
		}
	}

	// BFS (Breadth First Search) Busqueda en anchura	
	Node* BFS(const char* tagTarget)
	{
		std::queue<NodeVisit> q {};

		q.push({this, true});

		while (!q.empty())
		{
			Node* currentNode { q.front().node };
			q.pop();

			if (currentNode->getTag() == tagTarget) return currentNode;

			for (Node*& node : currentNode->m_children)
			{
				NodeVisit snode { node };
				
				if (!snode.visited)
				{
					snode.visited = true;

					q.push(snode);
				}
			}
		}
		return nullptr;
	}

	// DFS (Depth First Search) Busqueda en profundidad	
	Node* DFS(const char* tagTarget)
	{
		std::stack<NodeVisit> s{};

		s.push({ this, true });
		 
		while (!s.empty())
		{
			Node* currentNode { s.top().node };
			s.pop();

			if (currentNode->getTag() == tagTarget) return currentNode;

			for (Node*& node : currentNode->m_children)
			{
				NodeVisit snode { node };

				if (!snode.visited)
				{
					s.push(snode);
					snode.visited = true;
				}
			}
		}
		return nullptr;
	}	

private:	
	struct NodeVisit
	{
		Node* node = nullptr;
		bool  visited = false;
	};

private:
	[[nodiscard]] bool isInLayer(uint8_t idx) const;
	void setLayer(uint8_t idx, bool value);

	void init(SceneGraph* scenegraph, Node* parent);
	void destroyChild(Node* node);
	

private:
	std::string m_entity{}; // systems

	SceneGraph*		   m_sceneGraph { nullptr		 };
	Node*			   m_parent		{ nullptr		 };
	std::vector<Node*> m_children	{				 };
	uint32_t		   m_layers		{ Layer::Default };	
	std::string		   m_tag		{ "unnamed"		 };
	uint32_t		   m_depth		{ 0				 };
	//uint32_t		   m_nodeRef	{ 0 };
	//Node* m_search{ nullptr };

private:
	friend class SceneGraph;
};