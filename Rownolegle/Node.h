#include<vector>

#pragma once
class Node
{
public:
	int id; /*!< Id wezla */
	float x; /*!< Wspolrzedna x */
	float y; /*!< Wspolrzedna y */

public:
	Node(int _id, float _x, float _y);
	Node();
	~Node();
};

