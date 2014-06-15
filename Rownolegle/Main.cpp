#include <utility>
#include<iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include<time.h>
#include<math.h>
#include <iterator>
//#include<vector>
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.hpp>
#include"Element.h"
#include"Point.h"

using namespace std;

const int arraySize = 20;
const int maxValue = 10;

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

vector<cl_float> ep1x;
vector<cl_float> ep1y;
vector<cl_float> ep2x;
vector<cl_float> ep2y;
vector<cl_float> ep3x;
vector<cl_float> ep3y;
cl_int eSize;

vector<cl_float> px;
vector<cl_float> py;
cl_int pSize;
vector<cl_int> elId;

void checkPoints();
float sign(Point p1, Point p2, Point p3);
bool PointInTriangle(Point pt, Point v1, Point v2, Point v3);
inline void checkErr(cl_int err, const char* name);
void copyElementsToVector(vector<cl_float>* vec1, vector<cl_float>* vec2, vector<cl_float>* vec3, vector<cl_float>* vec4, vector<cl_float>* vec5, vector<cl_float>* vec6, vector<Element> elem);
void copyPointsToVector(vector<cl_float>* vec1, vector<cl_float>* vec2, vector<cl_int>* toZero, vector<Point> points);

void randomValues(cl_int* data, cl_int maxValue, int size)
{
	for (int i = 0; i < size; i++)
	{
		data[i] = rand() % maxValue;
	}
}

std::vector<cl_int> randomValues(cl_int maxValue, int size)
{
	std::vector<int> data;
	for (int i = 0; i < size; i++)
	{
		data.push_back(rand() % maxValue);
		//std::cout << "Size: " << size << " i: " << i << "\n";
		//std::cout << "sss: " << data.size() << "\n";
		//data[i] = 
	}
	return data;
}

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
	//t1 = clock();
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


	copyElementsToVector(&ep1x, &ep1y, &ep2x, &ep2y, &ep3x, &ep3y, elementsList);
	//cout<<"Elementy: " << ep1x.size() << "\n";
	eSize = ep1x.size();

	/*for each(Element e in elementsList){
	cout << e.id << " " << e.n1->id << " " << e.n2->id << " " << e.n3->id << "\n";
	}*/

	vector<Element*> foundedElements;
	bool isFound = false;
	Node* foundedNode = new Node();
	//int idToFind;
	cout << "Wygenerowano siatke:\n";
	//t2 = clock();
	//float diff((float)t2 - (float)t1);
	//cout << "Czas generowania: " << (diff / CLOCKS_PER_SEC) << "\n";

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
	a = (p2.y - p1.y) / (p2.x - p1.x);
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

	copyPointsToVector(&px, &py, &elId, pointsList);
	pSize = px.size();
	//cout<<"Psize: " << pSize << "\n";
	//cout << "Ilosc pkt: " << px.size() << "\n";

	segmentLength = sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	cout << "Dlugosc podanego odcinka: " << segmentLength << "\n";

	t1 = clock();

	/*for (int i = 0; i < pointsList.size(); ++i){
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
	}*/

	cl_int err;
	std::vector<cl::Platform> platformList;
	cl::Platform::get(&platformList);
	checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");

	std::cerr << "Platform number is: " << platformList.size() << std::endl;

	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	std::cerr << "Platform is by: " << platformVendor << "\n";

	cl_context_properties cprops[3] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)(platformList[0])(),
		0
	};

	cl::Context context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err);
	checkErr(err, "Context::Context()");

	/*std::vector<cl_int> av;
	std::vector<cl_int> bv;
	std::vector<cl_int> cv;

	int* A = new int[arraySize];
	int* B = new int[arraySize];
	int* out = new int[arraySize];

	randomValues(A, maxValue, arraySize);
	randomValues(B, maxValue, arraySize);

	av = randomValues(maxValue, arraySize);
	bv = randomValues(maxValue, arraySize);
	cv = randomValues(maxValue, arraySize);

	std::cout << "Size: " << av.size() << "\n";*/

	cl::Buffer bP1x(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP1y(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP2x(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP2y(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP3x(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP3y(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	//cl::Buffer bESize(context, CL_MEM_READ_ONLY, sizeof(cl_int));

	cl::Buffer bPx(context, CL_MEM_READ_ONLY, sizeof(cl_float)* pSize);
	cl::Buffer bPy(context, CL_MEM_READ_ONLY, sizeof(cl_float)* pSize);
	//cl::Buffer bPSize(context, CL_MEM_READ_ONLY, sizeof(cl_int));

	cl::Buffer outElId(context, CL_MEM_WRITE_ONLY, sizeof(cl_int)* pSize);

	//cl::Buffer inAv(context, CL_MEM_READ_ONLY, sizeof(int)* arraySize);
	//cl::Buffer inBv(context, CL_MEM_READ_ONLY, sizeof(int)* arraySize);

	std::vector<cl::Device> devices;
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	std::ifstream file("solution.cl");
	checkErr(file.is_open() ? CL_SUCCESS : -1, "solution.cl");

	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));
	cl::Program program(context, source);
	err = program.build(devices, "");
	checkErr(err, "Program.build()");

	cl::CommandQueue queue(context, devices[0], 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");

	queue.enqueueWriteBuffer(bP1x, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep1x[0]);
	queue.enqueueWriteBuffer(bP1y, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep1y[0]);
	queue.enqueueWriteBuffer(bP2x, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep2x[0]);
	queue.enqueueWriteBuffer(bP2y, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep2y[0]);
	queue.enqueueWriteBuffer(bP3x, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep3x[0]);
	queue.enqueueWriteBuffer(bP3y, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep3y[0]);
	//queue.enqueueWriteBuffer(bESize, CL_TRUE, 0, sizeof(cl_int), &eSize);

	queue.enqueueWriteBuffer(bPx, CL_TRUE, 0, sizeof(cl_float)* pSize, &px[0]);
	queue.enqueueWriteBuffer(bPy, CL_TRUE, 0, sizeof(cl_float)* pSize, &py[0]);
	//queue.enqueueWriteBuffer(bPSize, CL_TRUE, 0, sizeof(cl_int), &pSize);

	cl::Kernel kernel(program, "multiply", &err);
	kernel.setArg(0, bP1x);
	kernel.setArg(1, bP1y);
	kernel.setArg(2, bP2x);
	kernel.setArg(3, bP2y);
	kernel.setArg(4, bP3x);
	kernel.setArg(5, bP3y);
	kernel.setArg(6, eSize);
	kernel.setArg(7, bPx);
	kernel.setArg(8, bPy);
	kernel.setArg(9, pSize);
	kernel.setArg(10, outElId);
	
	const int WORKSPACE_MULT = 64;
	int workspace = 0;
	workspace = ((pSize / WORKSPACE_MULT)*WORKSPACE_MULT);
	if (pSize%WORKSPACE_MULT != 0) workspace += WORKSPACE_MULT;
	cout << "SPece: " << workspace << endl;
	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(workspace), cl::NDRange(WORKSPACE_MULT), NULL, &event);
	checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");

	event.wait();
	err = queue.enqueueReadBuffer(outElId, CL_TRUE, 0, sizeof(cl_int)* pSize, &elId[0]);
	checkErr(err, "ComamndQueue::enqueueReadBuffer()");
	/*for (int i = 0; i < av.size(); i++)
	{
	std::cout << av[i] << " * " << bv[i] << " = " << cv[i] << std::endl;
	}*/
	//cout << elId.size() << "\n";
	t2 = clock();
	float diff = ((float)t2 - (float)t1);
	for (int i = 0; i < elId.size(); i++){
		if (elId[i] != -1)cout << "Punkt nr. " << i << " nalezy do elementu o id: " << elId[i] << "\n";
		else cout << "Punkt nr. " << i << " znajduje sie poza siatka\n";
	}
	
	cout << "Czas obliczen: "<<(diff / CLOCKS_PER_SEC) << endl;
	std::cin.get();

	/*delete[] out;
	delete[] A;
	delete[] B;*/


	// OPENCL KONIEC



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

inline void checkErr(cl_int err, const char* name)
{
	if (err != CL_SUCCESS)
	{
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		std::cin.get();
		//exit(EXIT_FAILURE);
	}
}

void copyElementsToVector(vector<cl_float>* vec1, vector<cl_float>* vec2, vector<cl_float>* vec3, vector<cl_float>* vec4, vector<cl_float>* vec5, vector<cl_float>* vec6, vector<Element> elem){
	for (Element e : elem){
		vec1->push_back(e.n1->x);
		vec2->push_back(e.n1->y);
		vec3->push_back(e.n2->x);
		vec4->push_back(e.n2->y);
		vec5->push_back(e.n3->x);
		vec6->push_back(e.n3->y);
	}
}

void copyPointsToVector(vector<cl_float>* vec1, vector<cl_float>* vec2, vector<cl_int>* toZero, vector<Point> points){
	for (Point p : points){
		vec1->push_back(p.x);
		vec2->push_back(p.y);
		toZero->push_back(-1);
	}
}