#include<iostream>
#include<vector>
#include"Element.h"

using namespace std;

int nodesHorizontal;
int nodesVertical;
float width;
float height;
vector<Node> nodesList;

int main(){
	std::cout << "Podaj ilosc wezlow w szerokosci:" << std::endl;
	cin >> nodesHorizontal;
	std::cout << "Podaj ilosc wezlow w wysokosci:" << std::endl;
	cin >> nodesVertical;
	std::cout << "Podaj szerokosc siatki:" << std::endl;
	cin >> width;
	std::cout << "Podaj wysokosc siatki:" << std::endl;
	cin >> height;

	float horBreak = width / nodesHorizontal;
	float verBreak = height / nodesVertical;

	cout << horBreak<<" "<<verBreak << endl;

	int idTemp = 0;
	float xTemp = 0;
	float yTemp = 0;

	for (int i = 0; i < nodesVertical; i++){
		for (int j = 0; j < nodesHorizontal; j++){

		}
	}


	system("pause");

}