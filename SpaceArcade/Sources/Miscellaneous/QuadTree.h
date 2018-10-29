#pragma once
#include "../../Header.h"

struct QTRectangle
{
	int x, y;
	int Width, Height;
	int object_index;
};

class QuadTree
{
public:
	QuadTree();
	QuadTree(int pLevel, QTRectangle pBounds);

	~QuadTree();

	void clear();
	void reset(QTRectangle pBounds);

	//Splits the node into 4 subnodes
	void split();

	/*
		 Insert the object into the quadtree. If the node
		 exceeds the capacity, it will split and add all
		 objects to their corresponding nodes.
	*/
	void insert(QTRectangle pRect);

	/*
		 Return all objects that could collide with the given object
	*/
	void retrieve(std::vector<QTRectangle> &returnObjects, QTRectangle pRect);

	/*
		 Determine which node the object belongs to. -1 means
		 object cannot completely fit within a child node and is part
		 of the parent node
   */
	int getIndex(QTRectangle pRect);

	int level;
	bool freeNode;
	bool has_children;
	std::vector<QTRectangle> objects;
	QTRectangle bounds;
	QuadTree *nodes;

	int max_objects = 3;
	int max_levels = 7;
};