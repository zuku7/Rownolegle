#include "Node.h"


Node::Node(int _id, float _x, float _y){
	id = _id;
	x = _x;
	y = _y;
}

Node::Node(){
	id = NULL;
	x = NULL;
	y = NULL;
}

Node::~Node()
{
}

