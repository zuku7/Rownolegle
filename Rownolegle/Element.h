#include"Node.h"

#pragma once
class Element
{
public:
	int id;
	Node* n1;
	Node* n2;
	Node* n3;

public:
	Element(int _id, Node* _n1, Node* _n2, Node* _n3);
	~Element();
};

