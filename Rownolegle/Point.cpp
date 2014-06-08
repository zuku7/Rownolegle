#include "Point.h"


Point::Point(){
	x = 0;
	y = 0;
	elementId = -1;
}

Point::Point(float _x, float _y){
	x = _x;
	y = _y;
	elementId = -1;
}
Point::~Point()
{
}
