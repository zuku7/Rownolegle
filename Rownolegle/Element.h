#include"Node.h"

#pragma once
class Element
{
public:
	int id;  /*!< Id elementu */
	Node* n1; /*!< Wskaznik na pierwszy wezel elementu */
	Node* n2; /*!< Wskaznik na drugi wezel elementu */
	Node* n3; /*!< Wskaznik na trzeci wezel elementu */

public:
	Element(int _id, Node* _n1, Node* _n2, Node* _n3);
	~Element();
};

