#include "QuadTree.h"

QuadTree::QuadTree()
{
	freeNode = 1;
	has_children = false;
}

QuadTree::QuadTree(int pLevel, QTRectangle pBounds)
{
	level = pLevel;
	bounds = pBounds;
	nodes = new QuadTree[4];
	has_children = true;
	freeNode = 1;
}

QuadTree::~QuadTree()
{
	//clear();
}

void QuadTree::reset(QTRectangle pBounds)
{
	clear();

	bounds = pBounds;
	nodes = new QuadTree[4];
}

void QuadTree::split()
{
	int subWidth = (int)(bounds.Width / 2);
	int subHeight = (int)(bounds.Height / 2);
	int x = (int)bounds.x;
	int y = (int)bounds.y;

	QTRectangle rect;
	rect.x = x + subWidth;
	rect.y = y;
	rect.Width = subWidth;
	rect.Height = subHeight;

	nodes[0] = QuadTree(level + 1, rect);
	rect.x = x;
	nodes[1] = QuadTree(level + 1, rect);
	rect.y = y + subHeight;
	nodes[2] = QuadTree(level + 1, rect);
	rect.x = x + subWidth;
	nodes[3] = QuadTree(level + 1, rect);
}

void QuadTree::insert(QTRectangle pRect)
{
	if (nodes[0].freeNode != 1)
	{
		int index = getIndex(pRect);

		if (index != -1)
		{
			nodes[index].insert(pRect);
			return;
		}
	}

	objects.push_back(pRect);

	if (objects.size() > max_objects && level < max_levels)
	{
		if (nodes[0].freeNode == 1)
		{
			split();
			nodes[0].freeNode = 0;
		}

		int i = 0;
		while (i < objects.size())
		{
			int index = getIndex(objects[i]);
			if (index != -1) {
				nodes[index].insert(objects[i]);
				objects.erase(objects.begin() + i);

			}
			else
				i++;
		}
	}
}

void QuadTree::retrieve(std::vector<QTRectangle> &returnObjects, QTRectangle pRect)
{
	int index = getIndex(pRect);

	if (index != -1 && nodes[0].freeNode != 1) {
		nodes[index].retrieve(returnObjects, pRect);
	}

	for (int i = 0; i < objects.size(); ++i)
		if(objects[i].object_index != pRect.object_index)
			returnObjects.push_back(objects[i]);
}

int QuadTree::getIndex(QTRectangle pRect)
{
	int index = -1;
	double verticalMidpoint = bounds.x + (bounds.Width / 2);
	double horizontalMidpoint = bounds.y + (bounds.Height / 2);

	// Object can completely fit within the top quadrants
	bool topQuadrant = (pRect.y < horizontalMidpoint && pRect.y + pRect.Height < horizontalMidpoint);
	// Object can completely fit within the bottom quadrants
	bool bottomQuadrant = (pRect.y > horizontalMidpoint);

	// Object can completely fit within the left quadrants
	if (pRect.x < verticalMidpoint && pRect.x + pRect.Width < verticalMidpoint) {
		if (topQuadrant) {
			index = 1;
		}
		else if (bottomQuadrant) {
			index = 2;
		}
	}
	// Object can completely fit within the right quadrants
	else if (pRect.x > verticalMidpoint) {
		if (topQuadrant) {
			index = 0;
		}
		else if (bottomQuadrant) {
			index = 3;
		}
	}

	return index;
}

void QuadTree::clear()
{
	for (int i = 0; i < sizeof(nodes); i++)
	{
		if (nodes[i].has_children)
			nodes[i].clear();

	}
	delete[]nodes;

	objects.clear();
}