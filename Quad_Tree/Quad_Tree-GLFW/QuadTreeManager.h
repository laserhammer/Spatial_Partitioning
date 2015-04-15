#pragma once
#include <vector>

class InteractiveShape; 
class RenderShape;

struct QuadTreeNode
{
	std::vector<InteractiveShape*> shapes;
	RenderShape* outline;
	int children[4];
	int parent;

	bool active;
	bool hasChildren;
	unsigned int depth;
	float left;
	float right;
	float top;
	float bottom;
};

class QuadTreeManager
{
public:

	static void InitQuadTree(float left, float right, float top, float bottom, unsigned int maxDepth, unsigned int maxPerNode, RenderShape outlineTemplate);

	static void UpdateQuadtree();

	static void AddShape(InteractiveShape* shape);

	static void DumpData();

	static const std::vector<InteractiveShape*>& GetNearbyShapes(InteractiveShape* shape);

private:

	static void AddShape(InteractiveShape* shape, int startingNode);

	static int CheckShapeNodeCollide(InteractiveShape* shape, QuadTreeNode* node);

	static void InitChildren(int nodeIndex);

	static QuadTreeNode* InitNode(int depth, int parentIndex, int childNum, float left, float right, float top, float bottom);

	static void ActivateChildren(QuadTreeNode* parent);

	static void DeactivateNode(QuadTreeNode* node);

	static void ActivateNode(QuadTreeNode* node);

	static int GetDepthIndex(int depth);

	static std::vector<QuadTreeNode*> _quadTree;
	static std::vector<InteractiveShape*> _shapes;
	static unsigned int _maxDepth;
	static unsigned int _maxPerNode;
	static RenderShape _outlineTemplate;
};
