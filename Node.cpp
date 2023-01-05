#include "Node.h"
#include <stack>


Node::Node()
{
	
}

Node::~Node()
{
	//delete m_parent;

	//std::cout << m_children.size() << "\n";

	for (auto& child : m_children)
	{
		// std::cout << "delete " << m_children[i]->getTag() << "\n";
		delete child;
	}
}

void Node::enable() {
	if (!Node::isInLayer(Layer::Enum::Enabled)) 
		m_layers += Layer::Enum::Enabled;
}
void Node::disable() {
	if (Node::isInLayer(Layer::Enum::Enabled)) 
		m_layers -= Layer::Enum::Enabled;
}
bool Node::isEnabled() const { 
	return isInLayer(Layer::Enum::Enabled); 
}
void Node::enableRecursive() { 
	if (!Node::isInLayer(Layer::Enum::Recursive)) 
		m_layers += Layer::Enum::Recursive;
}
void Node::disableRecursive(){
	if (Node::isInLayer(Layer::Enum::Recursive)) 
		m_layers -= Layer::Enum::Recursive;
}
bool Node::isEnabledRecursive() const {	
	return Node::isInLayer(Layer::Enum::Recursive);
}
void Node::setTag(const char* tag) {
	m_tag = tag;
}
const std::string& Node::getTag() const {
	return m_tag;
}
uint32_t Node::getDepth() const {
	return m_depth;
}
Node* Node::getParent() {
	return m_parent;
}

const Node* Node::getParent() const {
	return m_parent;
}

Node* Node::createChild()
{	
	Node* newnode = new Node();
	newnode->m_parent = this;
	m_children.push_back(newnode);
	return m_children[m_depth++];
}

const Node* Node::childAt(size_t idx) const
{
	if (idx >= m_children.size()) return nullptr;
	return m_children[idx];
}

Node* Node::childAt(size_t idx)
{
	if (idx >= m_children.size()) return nullptr;
	return m_children[idx];
}

Node* Node::childByTag(const char* tag, bool recursive, Node* _node)
{
	if (recursive) {	
		size_t idx = 0;
		for (;;) {
			Node* node = childAt(idx++);			
			if (!node) return nullptr;
			if (node->getTag() == tag) return node;				
			node->childByTag(tag, true, _node);
		}
		return nullptr;
	}
	for (Node*& node : m_children) if (node->m_tag == tag) { return node;  }
	return nullptr;
}
size_t Node::numChildren() const
{
	return m_children.size();
}

uint32_t Node::getDepth()
{
	return m_depth;
}

const std::string& Node::getEntity()
{
	return m_entity;
}

//Node* Node::DFS(const char* target);

bool Node::isInLayer(uint8_t idx) const
{
	return (m_layers & idx);
}

void Node::setLayer(uint8_t idx, bool value)
{
	if (!isInLayer(idx) && value == true) m_layers += idx;
	if (isInLayer(idx) && value == false) m_layers -= idx;
}

void Node::init(SceneGraph* scenegraph, Node* parent)
{
	m_sceneGraph = scenegraph;
	m_parent = parent;
}

void Node::destroyChild(Node* node)
{

}


