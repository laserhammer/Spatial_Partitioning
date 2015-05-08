#include "QuadTreeManager.h"
#include "InteractiveShape.h"
#include "RenderShape.h"
#include "RenderManager.h"

std::vector<QuadTreeNode*> QuadTreeManager::_quadTree = std::vector<QuadTreeNode*>();
std::vector<InteractiveShape*> QuadTreeManager::_shapes = std::vector<InteractiveShape*>();
unsigned int QuadTreeManager::_maxDepth = 0;
unsigned int QuadTreeManager::_maxPerNode = 0;
RenderShape QuadTreeManager::_outlineTemplate;

// When the quad-tree manager is initialized, it instantiates the entire possible tree to avoid having to do a bunch of 
// time wasting news and deletes during runtime.
void QuadTreeManager::InitQuadTree(float left, float right, float top, float bottom, unsigned int maxDepth, unsigned int maxPerNode, RenderShape outlineTemplate)
{
	_maxPerNode = maxPerNode;
	_maxDepth = maxDepth;
	_outlineTemplate = outlineTemplate;
	_quadTree.resize(GetDepthIndex(maxDepth));
	_quadTree[0] = InitNode(0, 0, 0, left, right, top, bottom);
	int maxI = GetDepthIndex(maxDepth - 1);
	for (int i = 0; i < maxI; ++i)
	{
		InitChildren(i);
	}
}

// When updateing the tree, the manager goes through and deactivates every node in the tree and then reactivates the root.
// It then goes through the entire array of interactive shapes and adds them back into the tree. 
void QuadTreeManager::UpdateQuadtree()
{
	unsigned int treeSize = _quadTree.size();
	for (unsigned int i = 0; i < treeSize; ++i)
	{
		DeactivateNode(_quadTree[i]);
	}
	ActivateNode(_quadTree[0]);
	unsigned int shapesSize = _shapes.size();
	for (unsigned int i = 0; i < shapesSize; ++i)
	{
		AddShape(_shapes[i], 0);
	}
}

void QuadTreeManager::AddShape(InteractiveShape* shape)
{
	_shapes.push_back(shape);
}

// When the program ends, the manager has to go through and delete all of the nodes of quad tree that it instantiated
// during the init function.
void QuadTreeManager::DumpData()
{
	int i;
	while ((i = _quadTree.size()) > 0)
	{
		delete _quadTree[i - 1];
		_quadTree.pop_back(); 
	}
}

// Retrieves all the shapes that share a node with the shape passed in. It uses a method similar to when a shape is being
// added to the tree. When it gets to the lowest node that the argument shape collides with, it returns the array of shapes
// associated with that node.
const std::vector<InteractiveShape*>& QuadTreeManager::GetNearbyShapes(InteractiveShape* shape)
{
	unsigned int treeSize = _quadTree.size();
	for (unsigned int i = 0; i < treeSize;)
	{
		QuadTreeNode* currentNode = _quadTree[i];
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
				return _quadTree[currentNode->parent]->shapes;
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

// Adds the given shape to the quad tree beginnng at the node index passed in. Shapes are added to the first node that with which they have a successful collision
// If they only have a partial collision, they are added to that node's parent. If a node is at the bottom of the activated tree, and it exceeds the max number
// of shapes, then each of it's shapes are added back into the tree, passing that node's index as the starting node and that node's children are activated. 
void QuadTreeManager::AddShape(InteractiveShape* shape, int startingNode)
{
	unsigned int treeSize = _quadTree.size();
	for (unsigned int i = startingNode; i < treeSize;)
	{
		QuadTreeNode* currentNode = _quadTree[i];
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
			// This check prevens the issue of having shapes on the edge of the root node. Since the root node has no
			// parent nodes, any partial collisions with the root have to be considered full collisions. 
			if (currentNode->depth != 0)
			{
				_quadTree[currentNode->parent]->shapes.push_back(shape);
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

// Just an AABB collision
int QuadTreeManager::CheckShapeNodeCollide(InteractiveShape* shape, QuadTreeNode* node)
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
	float width = node->right - node->left;
	float height = node->top - node->bottom;
	colStatus += abs(dTop) < height && abs(dBot) < height && abs(dRight) < width && abs(dLeft) < width;
	colStatus += (dTop > 0 && dBot < 0 && dRight > 0 && dLeft < 0);
	return colStatus;
}

void QuadTreeManager::InitChildren(int nodeIndex)
{
	QuadTreeNode* node = _quadTree[nodeIndex];
	float midX = node->left + ((node->right - node->left) / 2.0f);
	float midY = node->bottom + ((node->top - node->bottom) / 2.0f);
	int childNum = node->children[0] - GetDepthIndex(node->depth);
	_quadTree[node->children[0]] = InitNode(node->depth + 1, nodeIndex, childNum, node->left, midX, node->top, midY);
	_quadTree[node->children[1]] = InitNode(node->depth + 1, nodeIndex, childNum + 1, midX, node->right, node->top, midY);
	_quadTree[node->children[2]] = InitNode(node->depth + 1, nodeIndex, childNum + 2, node->left, midX, midY, node->bottom);
	_quadTree[node->children[3]] = InitNode(node->depth + 1, nodeIndex, childNum + 3, midX, node->right, midY, node->bottom);
}

QuadTreeNode* QuadTreeManager::InitNode(int depth, int parentIndex, int childNum, float left, float right, float top, float bottom)
{
	QuadTreeNode* node = new QuadTreeNode();
	node->active = false;
	node->hasChildren = false;
	node->depth = depth;
	node->left = left;
	node->right = right;
	node->top = top;
	node->bottom = bottom;

	RenderShape* outline = new RenderShape(_outlineTemplate.vao(), _outlineTemplate.count(), _outlineTemplate.mode(), _outlineTemplate.shader(), _outlineTemplate.color());
	RenderManager::AddShape(outline);
	node->outline = outline;

	int base = GetDepthIndex(depth) + childNum * 4;
	node->children[0] = base;
	node->children[1] = base + 1;
	node->children[2] = base + 2;
	node->children[3] = base + 3;
	node->parent = parentIndex;

	ActivateNode(node);

	return node;
}

void QuadTreeManager::ActivateChildren(QuadTreeNode* parent)
{
	parent->hasChildren = true;
	ActivateNode(_quadTree[parent->children[0]]);
	ActivateNode(_quadTree[parent->children[1]]);
	ActivateNode(_quadTree[parent->children[2]]);
	ActivateNode(_quadTree[parent->children[3]]);
}

void QuadTreeManager::DeactivateNode(QuadTreeNode* node)
{
	node->active = false;
	node->shapes.clear();
	node->outline->transform().position.x = 10000.0f;
	node->outline->transform().position.y = 10000.0f;
}

void QuadTreeManager::ActivateNode(QuadTreeNode* node)
{
	node->active = true;
	node->hasChildren = false;
	node->outline->transform().position.x = (node->left + node->right) / 2.0f;
	node->outline->transform().position.y = (node->top + node->bottom) / 2.0f;
	node->outline->transform().scale.x = (node->right - node->left) / 2.0f;
	node->outline->transform().scale.y = (node->top - node->bottom) / 2.0f;
}

int QuadTreeManager::GetDepthIndex(int depth)
{
	float depthf = (float)depth;
	float retf = 0.0f;
	for (float i = 0.0f; i <= depthf; i += 1.0f)
	{
		retf += powf(4.0f, i);
	}
	int ret = (int)retf;
	return  ret;
}

