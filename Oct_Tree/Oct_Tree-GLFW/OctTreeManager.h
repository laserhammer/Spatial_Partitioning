#pragma once
#include <vector>

class InteractiveShape;
class RenderShape;

struct OctTreeNode
{
	std::vector<InteractiveShape*> shapes;
	RenderShape* outline;
	int children[8];
	int parent;

	bool active;
	bool hasChildren;
	unsigned int depth;
	float left;
	float right;
	float top;
	float bottom;
	float front;
	float back;
};

class OctTreeManager
{
public:

	static void InitOctTree(float left, float right, float top, float bottom, float front, float back, unsigned int maxDepth, unsigned int maxPerNode, RenderShape outlineTemplate);

	static void UpdateOctTree();

	static void AddShape(InteractiveShape* shape);

	static void DumpData();

	static const std::vector<InteractiveShape*>& GetNearbyShapes(InteractiveShape* shape);

private:

	static void AddShape(InteractiveShape* shape, int startingNode);

	static int CheckShapeNodeCollide(InteractiveShape* shape, OctTreeNode* node);

	static void InitChildren(int nodeIndex);

	static OctTreeNode* InitNode(int depth, int parentIndex, int childNum, float left, float right, float top, float bottom, float front, float back);

	static void ActivateChildren(OctTreeNode* parent);

	static void DeactivateNode(OctTreeNode* node);

	static void ResetTree();

	static void ActivateNode(OctTreeNode* node);

	static int GetDepthIndex(int depth);

	static std::vector<OctTreeNode*> _octTree;
	static std::vector<InteractiveShape*> _shapes;
	static unsigned int _maxDepth;
	static unsigned int _maxPerNode;
	static RenderShape _outlineTemplate;
};
