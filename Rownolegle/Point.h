#pragma once
#include"Node.h"
class Point : public Node
{
public:

	//float x;
	//float y;
	int elementId; /*!< Id elementu do ktorego nalezy punkt */

	Point();
	Point(float _x, float _y);
	~Point();
};

