#include<iostream>
#include<time.h>
#include<math.h>
//#include<vector>
#include"Element.h"
#include"Point.h"

using namespace std;

int nodesHorizontal;
int nodesVertical;
float width;
float height;
vector<Node> nodesList;
vector<Element> elementsList;
clock_t t1, t2;
Point p1, p2;
int numberOfPoints;
float segmentLength;
vector<Point> pointsList;
/*Node findNodeById(vector<Node> nodesList, int nodeId){
for each(Node n in nodesList){
//if (n.id == nodeId) return n;
}
return Node();
}*/

void checkPoints();
float sign(Point p1, Point p2, Point p3);
bool PointInTriangle(Point pt, Point v1, Point v2, Point v3);

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
	t1 = clock();
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
	t2 = clock();
	float diff((float)t2 - (float)t1);
	cout << "Czas generowania: " << (diff/CLOCKS_PER_SEC) << "\n";

	cout << "Ilosc wezlow: " << nodesList.size() << "\n";
	cout << "Ilosc elementow: " << elementsList.size() << "\n";
	
	p1 = Point();
	p2 = Point();
	cout << "Podaj wspolrzedna x pierwszego punktu\n";
	cin >> p1.x;
	cout << "Podaj wspolrzedna y pierwszego punktu\n";
	cin >> p1.y;
	cout << "Podaj wspolrzedna x drugiego punktu\n";
	cin >> p2.x;
	cout << "Podaj wspolrzedna y drugiego punktu\n";
	cin >> p2.y;	
	
	cout << "Podaj ilosc punktow rozlozonych na linii\n";
	cin >> numberOfPoints;

	int a, b;
	a = (p2.y - p1.y) / (p2.x - p1.x) ;
	b = p1.y - a*p1.x;

	float xChange = (p1.x - p2.x) / (numberOfPoints - 1);
	float yChange = (p1.y - p2.y) / (numberOfPoints - 1);

	float tempX = p1.x - xChange;
	float tempY = p1.y - yChange;

	/*if (p1.x > p2.x){
		tempX = p1.x - p2.x;
	}*/

	pointsList.push_back(p1);

	for (int i = 1; i < numberOfPoints - 1; ++i){
		pointsList.push_back(Point(tempX, tempY));
		tempX -= xChange;
		tempY -= yChange;
	}

	pointsList.push_back(p2);


	segmentLength = sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	cout << "Dlugosc podanego odcinka: " << segmentLength << "\n";

	t1 = clock();
	for (int i = 0; i < pointsList.size(); ++i){
	//for (Point p : pointsList){
		//cout << p.x << " "<<p.y << "\n";
		for (Element e : elementsList){
			if (PointInTriangle(pointsList[i], Point(e.n1->x, e.n1->y), Point(e.n2->x, e.n2->y), Point(e.n3->x, e.n3->y))){
				pointsList[i].elementId = e.id;
				//cout << pointsList[i].x << " " << pointsList[i].y << "\n";
				break;
			}
		}
	}

	t2 = clock();
	diff=(float)t2 - (float)t1;
	cout << "Czas obliczen: " << (diff / CLOCKS_PER_SEC) << "\n";

	for (int i = 0; i < pointsList.size();++i){
		if (pointsList[i].elementId!=-1) cout << "Punkt nr. " << i << " nalezy do elementu o id: " << pointsList[i].elementId << "\n";
		else cout << "Punkt nr. " << i << " znajduje sie poza siatka\n";
	}

	//t1 = clock();

	/*for (int i = 0; i < elementsList.size(); i++){
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

	for each(Element e in elementsList){
	if (e.n1->id == idTemp || e.n2->id == idTemp || e.n3->id == idTemp) foundedElements.push_back(&e);
	cout << foundedElements[foundedElements.size()-1]->id << "\n";
	}

	//cout << foundedElements.size() << endl;
	t2 = clock();
	diff = ((float)t2 - (float)t1);
	cout << "Czas przeszukiwania: " << (diff / CLOCKS_PER_SEC) << "\n";
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
	}*/

	system("pause");

}

void checkPoints(){

	for (Point p : pointsList){

	}

}

float sign(Point p1, Point p2, Point p3){
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle(Point pt, Point v1, Point v2, Point v3){
	bool b1, b2, b3;

	b1 = sign(pt, v1, v2) < 0.0f;
	b2 = sign(pt, v2, v3) < 0.0f;
	b3 = sign(pt, v3, v1) < 0.0f;

	return ((b1 == b2) && (b2 == b3));
}