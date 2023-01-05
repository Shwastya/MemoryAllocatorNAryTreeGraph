#include "SceneGraph.h"
#include "Node.h"

SceneGraph::SceneGraph()
{
	m_root = new Node();
	m_root->init(this, nullptr);
	m_root->setTag("Root");
}

SceneGraph::~SceneGraph()
{
	delete m_root;
}

Node* SceneGraph::getRoot()
{
	return m_root;
}

Node* SceneGraph::createRootChild()
{
	return m_root->createChild();
}

uint32_t SceneGraph::addNodeCount()
{
	return m_nodeCount.fetch_add(1);
}

void SceneGraph::beforeNodeIsDestroyed(Node* node)
{
	// TODO remeve component and all components froms system
}

void SceneGraph::update()
{
	// update all systems

	// Temporal DEBUG
	m_root->debugVisit();
}