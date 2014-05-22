#include<iostream>
//#include<vector>
#include"Element.h"

using namespace std;

int nodesHorizontal;
int nodesVertical;
float width;
float height;
vector<Node> nodesList;
vector<Element> elementsList;

/*Node findNodeById(vector<Node> nodesList, int nodeId){
	for each(Node n in nodesList){
		//if (n.id == nodeId) return n;
	}
	return Node();
}*/

int main(){
	std::cout << "Podaj ilosc wezlow w szerokosci:" << std::endl;
	cin >> nodesHorizontal;
	std::cout << "Podaj ilosc wezlow w wysokosci:" << std::endl;
	cin >> nodesVertical;
	std::cout << "Podaj szerokosc siatki:" << std::endl;
	cin >> width;
	std::cout << "Podaj wysokosc siatki:" << std::endl;
	cin >> height;

	cout << "Trwa generowanie siatki...\n";

	float horBreak = width / (nodesHorizontal - 1);
	float verBreak = height / (nodesVertical - 1);

	//cout << horBreak<<" "<<verBreak << endl;

	int idTemp = 0;
	float xTemp = 0;
	float yTemp = 0;

	for (int i = 0; i < nodesVertical; i++){
		xTemp = 0;
		for (int j = 0; j < nodesHorizontal; j++){
			nodesList.push_back(Node(idTemp, xTemp, yTemp));
			xTemp += horBreak;
			++idTemp;
		}
		yTemp += verBreak;
	}

	/*for each(Node n in nodesList){
		cout << n.id << " " << n.x << " " << n.y << "\n";
		}*/

	idTemp = 0;

	for (int i = 0; i < (nodesList.size() - nodesHorizontal); ++i){
		if (i%nodesHorizontal == 0) elementsList.push_back(Element(idTemp++, &nodesList[i], &nodesList[i + 1], &nodesList[i + nodesHorizontal]));
		else if (i%nodesHorizontal == nodesHorizontal - 1){
			elementsList.push_back(Element(idTemp++, &nodesList[i], &nodesList[i + nodesHorizontal - 1], &nodesList[i + nodesHorizontal]));
			continue;
		}
		else{
			elementsList.push_back(Element(idTemp++, &nodesList[i], &nodesList[i + 1], &nodesList[i + nodesHorizontal]));
			elementsList.push_back(Element(idTemp++, &nodesList[i], &nodesList[i + nodesHorizontal - 1], &nodesList[i + nodesHorizontal]));
		}
	}



	//cout << elementsList.size() << "\n";

	/*for each(Element e in elementsList){
		cout << e.id << " " << e.n1->id << " " << e.n2->id << " " << e.n3->id << "\n";
		}*/

	vector<Element*> foundedElements;
	bool isFound = false;
	Node* foundedNode = new Node();
	//int idToFind;
	cout << "Wygenerowano siatke:\n";
	cout << "Ilosc wezlow: " << nodesList.size() << "\n";
	cout << "Ilosc elementow: " << elementsList.size() << "\n";
	cout << "Podaj id szukanego wezla\n";
	cin >> idTemp;

	for (int i = 0; i < elementsList.size(); i++){
		if (elementsList[i].n1->id == idTemp || elementsList[i].n2->id == idTemp || elementsList[i].n3->id == idTemp){
			foundedElements.push_back(&elementsList[i]);
			if (!isFound){
				if (elementsList[i].n1->id == idTemp) foundedNode = elementsList[i].n1;
				else if (elementsList[i].n2->id == idTemp) foundedNode = elementsList[i].n2;
				else foundedNode = elementsList[i].n3;
				isFound = true;
			}

		}
	}

	/*for each(Element e in elementsList){
		if (e.n1->id == idTemp || e.n2->id == idTemp || e.n3->id == idTemp) foundedElements.push_back(&e);
		cout << foundedElements[foundedElements.size()-1]->id << "\n";
		}*/

	//cout << foundedElements.size() << endl;
	if (foundedElements.size() > 0){
		cout << "Zadany wezel nalezy do: " << foundedElements.size() << " elementow. Ich id to:\n";

		for each(Element* e in foundedElements){
			cout << e->id << "\n";
		}

		cout << "Wspolrzedne zadanego wezla to:\n";
		cout << "x: " << foundedNode->x << "\n";
		cout << "y: " << foundedNode->y << "\n";
	}
	else{
		cout << "Nie odnaleziono wezla\n";
	}

	system("pause");

}