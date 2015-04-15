#include "OctTreeManager.h"
#include "InteractiveShape.h"
#include "RenderShape.h"
#include "RenderManager.h"
#include <stack>

std::vector<OctTreeNode*> OctTreeManager::_octTree = std::vector<OctTreeNode*>();
std::vector<InteractiveShape*> OctTreeManager::_shapes = std::vector<InteractiveShape*>();
unsigned int OctTreeManager::_maxDepth = 0;
unsigned int OctTreeManager::_maxPerNode = 0;
RenderShape OctTreeManager::_outlineTemplate;

void OctTreeManager::InitOctTree(float left, float right, float top, float bottom, float front, float back, unsigned int maxDepth, unsigned int maxPerNode, RenderShape outlineTemplate)
{
	_maxPerNode = maxPerNode;
	_maxDepth = maxDepth;
	_outlineTemplate = outlineTemplate;
	_octTree.resize(GetDepthIndex(maxDepth));
	_octTree[0] = InitNode(0, 0, 0, left, right, top, bottom, front, back);
	int maxI = GetDepthIndex(maxDepth - 1);
	for (int i = 0; i < maxI; ++i)
	{
		InitChildren(i);
	}
}

void OctTreeManager::UpdateOctTree()
{
	ResetTree();
	ActivateNode(_octTree[0]);
	unsigned int shapesSize = _shapes.size();
	for (unsigned int i = 0; i < shapesSize; ++i)
	{
		AddShape(_shapes[i], 0);
	}
}

void OctTreeManager::AddShape(InteractiveShape* shape)
{
	_shapes.push_back(shape);
}

void OctTreeManager::DumpData()
{
	int i;
	while ((i = _octTree.size()) > 0)
	{
		delete _octTree[i - 1];
		_octTree.pop_back();
	}
}

const std::vector<InteractiveShape*>& OctTreeManager::GetNearbyShapes(InteractiveShape* shape)
{
	unsigned int treeSize = _octTree.size();
	for (unsigned int i = 0; i < treeSize;)
	{
		OctTreeNode* currentNode = _octTree[i];
		int result = CheckShapeNodeCollide(shape, currentNode);
		// No collision
		if (result == 0)
		{
			++i;
			continue;
		}
		// Partial collision
		if (result == 1)
		{
			if (currentNode->depth != 0)
			{
				return _octTree[currentNode->parent]->shapes;
			}
			else
			{
				result = 2;
			}

		}
		// Full collision
		if (result == 2)
		{

			if (!currentNode->hasChildren)
			{
				return currentNode->shapes;
			}
			else
			{
				i = currentNode->children[0];
			}
		}
	}
}

void OctTreeManager::AddShape(InteractiveShape* shape, int startingNode)
{
	unsigned int treeSize = _octTree.size();
	for (unsigned int i = startingNode; i < treeSize;)
	{
		OctTreeNode* currentNode = _octTree[i];
		int result = CheckShapeNodeCollide(shape, currentNode);
		// No collision
		if (result == 0)
		{
			++i;
			continue;
		}
		// Partial collision
		if (result == 1)
		{
			if (currentNode->depth != 0)
			{
				_octTree[currentNode->parent]->shapes.push_back(shape);
				break;
			}
			else
			{
				result = 2;
			}

		}
		// Full collision
		if (result == 2)
		{
			if (currentNode->shapes.size() >= _maxPerNode  && currentNode->depth < _maxDepth)
			{
				if (!currentNode->hasChildren)
				{
					ActivateChildren(currentNode);
					// Add all the shapes in the current node to the current node's children
					std::vector<InteractiveShape*> shapesTemp = currentNode->shapes;
					currentNode->shapes.clear();
					unsigned int size = shapesTemp.size();
					for (unsigned int j = 0; j < size; ++j)
					{
						AddShape(shapesTemp[j], currentNode->children[0]);
					}
				}
				i = currentNode->children[0];
			}
			else
			{
				if (!currentNode->hasChildren)
				{
					currentNode->shapes.push_back(shape);
					break;
				}
				else
				{
					i = currentNode->children[0];
				}
			}
		}
	}
}

int OctTreeManager::CheckShapeNodeCollide(InteractiveShape* shape, OctTreeNode* node)
{
	// 0 = no collision
	// 1 = partial collision
	// 2 = full collision
	int colStatus = 0;
	Collider col = shape->collider();
	float dTop = node->top - (col.y + col.height / 2.0f);
	float dBot = node->bottom - (col.y - col.height / 2.0f);
	float dLeft = node->left - (col.x - col.width / 2.0f);
	float dRight = node->right - (col.x + col.width / 2.0f);
	float dFront = node->front - (col.z - col.depth / 2.0f);
	float dBack = node->back - (col.z + col.depth / 2.0f);
	float width = node->right - node->left;
	float height = node->top - node->bottom;
	float depth = node->front - node->back;
	colStatus += abs(dTop) < height && abs(dBot) < height && abs(dRight) < width && abs(dLeft) < width && abs(dFront) < depth && abs(dBack) < depth;
	colStatus += dTop > 0 && dBot < 0 && dRight > 0 && dLeft < 0 && dFront > 0 && dBack < 0;
	return colStatus;
}

void OctTreeManager::InitChildren(int nodeIndex)
{
	OctTreeNode* node = _octTree[nodeIndex];
	float midX = node->left + ((node->right - node->left) / 2.0f);
	float midY = node->bottom + ((node->top - node->bottom) / 2.0f);
	float midZ = node->back + ((node->front - node->back) / 2.0f);
	int childNum = node->children[0] - GetDepthIndex(node->depth);
	_octTree[node->children[0]] = InitNode(node->depth + 1, nodeIndex, childNum, node->left, midX, node->top, midY, node->front, midZ);
	_octTree[node->children[1]] = InitNode(node->depth + 1, nodeIndex, childNum + 1, midX, node->right, node->top, midY, node->front, midZ);
	_octTree[node->children[2]] = InitNode(node->depth + 1, nodeIndex, childNum + 2, node->left, midX, midY, node->bottom, node->front, midZ);
	_octTree[node->children[3]] = InitNode(node->depth + 1, nodeIndex, childNum + 3, midX, node->right, midY, node->bottom, node->front, midZ);
	_octTree[node->children[4]] = InitNode(node->depth + 1, nodeIndex, childNum + 4, node->left, midX, node->top, midY, midZ, node->back);
	_octTree[node->children[5]] = InitNode(node->depth + 1, nodeIndex, childNum + 5, midX, node->right, node->top, midY, midZ, node->back);
	_octTree[node->children[6]] = InitNode(node->depth + 1, nodeIndex, childNum + 6, node->left, midX, midY, node->bottom, midZ, node->back);
	_octTree[node->children[7]] = InitNode(node->depth + 1, nodeIndex, childNum + 7, midX, node->right, midY, node->bottom, midZ, node->back);
}

OctTreeNode* OctTreeManager::InitNode(int depth, int parentIndex, int childNum, float left, float right, float top, float bottom, float front, float back)
{
	OctTreeNode* node = new OctTreeNode();
	node->active = false;
	node->hasChildren = false;
	node->depth = depth;
	node->left = left;
	node->right = right;
	node->top = top;
	node->bottom = bottom;
	node->front = front;
	node->back = back;

	RenderShape* outline = new RenderShape(_outlineTemplate.vao(), _outlineTemplate.count(), _outlineTemplate.mode(), _outlineTemplate.shader(), _outlineTemplate.color());
	RenderManager::AddShape(outline);
	node->outline = outline;
	node->outline->active() = false;

	int base = GetDepthIndex(depth) + childNum * 8;
	node->children[0] = base;
	node->children[1] = base + 1;
	node->children[2] = base + 2;
	node->children[3] = base + 3;
	node->children[4] = base + 4;
	node->children[5] = base + 5;
	node->children[6] = base + 6;
	node->children[7] = base + 7;
	node->parent = parentIndex;

	node->outline->transform().position.x = (node->left + node->right) / 2.0f;
	node->outline->transform().position.y = (node->top + node->bottom) / 2.0f;
	node->outline->transform().position.z = (node->front + node->back) / 2.0f;
	node->outline->transform().scale.x = (node->right - node->left) / 2.0f;
	node->outline->transform().scale.y = (node->top - node->bottom) / 2.0f;
	node->outline->transform().scale.z = (node->front - node->back) / 2.0f;

	return node;
}

void OctTreeManager::ActivateChildren(OctTreeNode* parent)
{
	parent->hasChildren = true;
	ActivateNode(_octTree[parent->children[0]]);
	ActivateNode(_octTree[parent->children[1]]);
	ActivateNode(_octTree[parent->children[2]]);
	ActivateNode(_octTree[parent->children[3]]);
	ActivateNode(_octTree[parent->children[4]]);
	ActivateNode(_octTree[parent->children[5]]);
	ActivateNode(_octTree[parent->children[6]]);
	ActivateNode(_octTree[parent->children[7]]);
}

void OctTreeManager::ResetTree()
{
	std::stack<int> stack;
	stack.push(0);

	while (!stack.empty())
	{
		OctTreeNode* node = _octTree[stack.top()];
		stack.pop();

		if (node->active)
		{
			node->active = false;
			node->shapes.clear();
			node->outline->active() = false;

			if (node->hasChildren)
			{
				stack.push(node->children[0]);
				stack.push(node->children[1]);
				stack.push(node->children[2]);
				stack.push(node->children[3]);
				stack.push(node->children[4]);
				stack.push(node->children[5]);
				stack.push(node->children[6]);
				stack.push(node->children[7]);
			}
		}
	}
}

void OctTreeManager::ActivateNode(OctTreeNode* node)
{
	node->active = true;
	node->outline->active() = true;
	node->hasChildren = false;
}

int OctTreeManager::GetDepthIndex(int depth)
{
	float depthf = (float)depth;
	float retf = 0.0f;
	for (float i = 0.0f; i < depthf || i == depthf; i += 1.0f)
	{
		retf += powf(8.0f, i);
	}
	int ret = (int)retf;
	return  ret;
}

