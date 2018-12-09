#pragma once
/*
	* Space Arcade Game
	* Copyright (C) 2018 Paul Razumov.
	
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
	LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	* Source:
	*  https://github.com/pavelrazumoff/SpaceArcade_Game
*/

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