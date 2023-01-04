#pragma once
#include <cstdint>
#include <atomic>

class Node;
class SceneGraph
{
public:
	SceneGraph();
	~SceneGraph();

	SceneGraph(const SceneGraph&) = default;
	SceneGraph& operator=(const SceneGraph&) = default;

	SceneGraph(SceneGraph&&) noexcept = default;
	SceneGraph& operator=(SceneGraph&&) noexcept = default;

	Node* getRoot();
	Node* createRootChild();
	uint32_t addNodeCount();

	void beforeNodeIsDestroyed(Node* node);

	void update();

private:
	Node* m_root { nullptr };
	std::atomic<uint32_t> m_nodeCount = { 1 };
};