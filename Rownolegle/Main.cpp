#include<iostream>
#include <fstream>
#include<time.h>
#include<math.h>
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.hpp>
#include"Element.h"
#include"Point.h"

using namespace std;

const float SIZE_GPU = 0.5; /*!< Procent udzialu karty graficznej w pracy */

int nodesHorizontal; /*!< Ilosc wezlow w poziomie */
int nodesVertical; /*!< Ilosc wezlow w pionie */
float width; /*!< Szerokosc siatki */
float height; /*!< Wysokosc siatki */
vector<Node> nodesList; /*!< Vector wezlow */
vector<Element> elementsList; /*!< Vector elementow */
Point p1; /*!< Pierwszy punkt w odcinku */
Point p2; /*!< Ostatni punkt w odcinku */
int numberOfPoints; /*!< Ilosc punktow */

vector<Point> pointsList; /*!< Vector szukanych punktow */

vector<cl_float> ep1x; /*!< Wspolrzedne x pierwszego wezla elementow - kopia dla kernela */
vector<cl_float> ep1y; /*!< Wspolrzedne y pierwszego wezla elementow - kopia dla kernela */
vector<cl_float> ep2x; /*!< Wspolrzedne x drugiego wezla elementow - kopia dla kernela */
vector<cl_float> ep2y; /*!< Wspolrzedne y drugiego wezla elementow - kopia dla kernela */
vector<cl_float> ep3x; /*!< Wspolrzedne x trzeciego wezla elementow - kopia dla kernela */
vector<cl_float> ep3y; /*!< Wspolrzedne y trzeciego wezla elementow - kopia dla kernela */
cl_int eSize; /*!< Wielkosc vectora elementow - kopia dla kernela */

vector<cl_float> px; /*!< Wspolrzedne x punktow - kopia dla kernela */
vector<cl_float> py; /*!< Wspolrzedne y punktow - kopia dla kernela */
cl_int pSize; /*!< Wielkosc vectora punktow - kopia dla kernela */
vector<cl_int> elId; /*!< Vector wynikow zwrocony przez kernel */

/**
* Sprawdza czy dana funkcja OpenCl wykonala sie poprawnie.
* @param err - zmienna z numerem bledu.
* @param name - zmienna z nazwa bledu.
*/
inline void checkErr(cl_int err, const char* name);

/**
* Kopieje vector elementow na tablice float.
* @param vec1-vec6 - tablice float.
* @param elem - tablica elementow.
*/
void copyElementsToVector(vector<cl_float>* vec1, vector<cl_float>* vec2, vector<cl_float>* vec3, vector<cl_float>* vec4, vector<cl_float>* vec5, vector<cl_float>* vec6, vector<Element> elem);

/**
* Kopieje vector punktow na tablice float.
* @param vec1-vec2 - tablice float.
* @param toZero - tablica float, w ktorej bedzie znajdowal sie wynik programu.
* @param points - tablica punktow.
*/
void copyPointsToVector(vector<cl_float>* vec1, vector<cl_float>* vec2, vector<cl_int>* toZero, vector<Point> points);

/**
* Kopieje zwrocony vector id do vectora punktow.
* @param ids - vector id.
* @param points - vector punktow.
*/
void copyVectorToPoints(vector<cl_int> ids, vector<Point>* points);

/**
* Zeruje tymczasowe vectory uzywane przez opencl.
*/
void clearTempVectors();

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

	cout << "Wygenerowano siatke:\n";
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

	pointsList.push_back(p1);

	for (int i = 1; i < numberOfPoints - 1; ++i){
		pointsList.push_back(Point(tempX, tempY));
		tempX -= xChange;
		tempY -= yChange;
	}

	pointsList.push_back(p2);

	copyPointsToVector(&px, &py, &elId, pointsList);
	pSize = px.size();

	float segmentLength = sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));


	cl_int pSizeGPU = pSize*SIZE_GPU;
	//cout << "eSizeG: " << pSizeGPU << "\n";
	int pSizeCPU = pSize - pSizeGPU;
	clock_t t1, t2;
	t1 = clock();

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

	cl::Context context(CL_DEVICE_TYPE_ALL, cprops, NULL, NULL, &err);
	checkErr(err, "Context::Context()");

	cl::Buffer bP1x(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP1y(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP2x(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP2y(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP3x(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);
	cl::Buffer bP3y(context, CL_MEM_READ_ONLY, sizeof(cl_float)* eSize);

	cl::Buffer bPx(context, CL_MEM_READ_ONLY, sizeof(cl_float)* pSize);
	cl::Buffer bPy(context, CL_MEM_READ_ONLY, sizeof(cl_float)* pSize);

	cl::Buffer outElId(context, CL_MEM_WRITE_ONLY, sizeof(cl_int)* pSize);

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

	cl::CommandQueue queueCPU(context, devices[1], 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");
	

	queue.enqueueWriteBuffer(bP1x, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep1x[0]);
	queue.enqueueWriteBuffer(bP1y, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep1y[0]);
	queue.enqueueWriteBuffer(bP2x, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep2x[0]);
	queue.enqueueWriteBuffer(bP2y, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep2y[0]);
	queue.enqueueWriteBuffer(bP3x, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep3x[0]);
	queue.enqueueWriteBuffer(bP3y, CL_TRUE, 0, sizeof(cl_float)* eSize, &ep3y[0]);
	//queue.enqueueWriteBuffer(bESize, CL_TRUE, 0, sizeof(cl_int), &eSize);

	queue.enqueueWriteBuffer(bPx, CL_TRUE, 0, sizeof(cl_float)* pSizeGPU, &px[0]);
	queue.enqueueWriteBuffer(bPy, CL_TRUE, 0, sizeof(cl_float)* pSizeGPU, &py[0]);
	//queue.enqueueWriteBuffer(bPSize, CL_TRUE, 0, sizeof(cl_int), &pSize);
	//queue.enqueueWriteBuffer(bPSize, CL_TRUE, 0, sizeof(cl_int), &pSize);

	queueCPU.enqueueWriteBuffer(bP1x, CL_TRUE, 0, sizeof(cl_float)* pSizeCPU, &ep1x[0]);
	queueCPU.enqueueWriteBuffer(bP1y, CL_TRUE, 0, sizeof(cl_float)* pSizeCPU, &ep1y[0]);
	queueCPU.enqueueWriteBuffer(bP2x, CL_TRUE, 0, sizeof(cl_float)* pSizeCPU, &ep2x[0]);
	queueCPU.enqueueWriteBuffer(bP2y, CL_TRUE, 0, sizeof(cl_float)* pSizeCPU, &ep2y[0]);
	queueCPU.enqueueWriteBuffer(bP3x, CL_TRUE, 0, sizeof(cl_float)* pSizeCPU, &ep3x[0]);
	queueCPU.enqueueWriteBuffer(bP3y, CL_TRUE, 0, sizeof(cl_float)* pSizeCPU, &ep3y[0]);
	//queue.enqueueWriteBuffer(bESize, CL_TRUE, 0, sizeof(cl_int), &eSize);

	queueCPU.enqueueWriteBuffer(bPx, CL_TRUE, 0, sizeof(cl_float)* pSizeCPU, &px[pSizeGPU]);
	queueCPU.enqueueWriteBuffer(bPy, CL_TRUE, 0, sizeof(cl_float)* pSizeCPU, &py[pSizeGPU]);
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



	cl::Kernel kernelCPU(program, "multiply", &err);
	kernelCPU.setArg(0, bP1x);
	kernelCPU.setArg(1, bP1y);
	kernelCPU.setArg(2, bP2x);
	kernelCPU.setArg(3, bP2y);
	kernelCPU.setArg(4, bP3x);
	kernelCPU.setArg(5, bP3y);
	kernelCPU.setArg(6, eSize);
	kernelCPU.setArg(7, bPx);
	kernelCPU.setArg(8, bPy);
	kernelCPU.setArg(9, pSizeCPU);
	kernelCPU.setArg(10, outElId);
	
	const int WORKSPACE_MULT = 64;
	int workspace = 0;
	workspace = ((pSizeGPU / WORKSPACE_MULT)*WORKSPACE_MULT);
	if (pSizeGPU%WORKSPACE_MULT != 0) workspace += WORKSPACE_MULT;
	//cout << "SPece: " << workspace << endl;
	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(workspace), cl::NDRange(WORKSPACE_MULT), NULL, &event);
	checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");

	int workspaceCPU = ((pSizeCPU / WORKSPACE_MULT)*WORKSPACE_MULT);
	if (pSizeCPU%WORKSPACE_MULT != 0) workspaceCPU += WORKSPACE_MULT;
	//cout << "SPeceCPU: " << workspaceCPU << endl;
	cl::Event eventCPU;
	err = queueCPU.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(workspaceCPU), cl::NDRange(WORKSPACE_MULT), NULL, &eventCPU);
	checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");

	event.wait();
	eventCPU.wait();
	err = queue.enqueueReadBuffer(outElId, CL_TRUE, 0, sizeof(cl_int)* pSizeGPU, &elId[0]);
	checkErr(err, "ComamndQueue::enqueueReadBuffer()");
	err = queueCPU.enqueueReadBuffer(outElId, CL_TRUE, 0, sizeof(cl_int)* pSizeCPU, &elId[pSizeGPU]);
	checkErr(err, "ComamndQueue::enqueueReadBuffer()");

	t2 = clock();
	float diff = ((float)t2 - (float)t1);
	for (int i = 0; i < elId.size(); i++){
		if (elId[i] != -1)cout << "Punkt nr. " << i << " nalezy do elementu o id: " << elId[i] << "\n";
		else cout << "Punkt nr. " << i << " znajduje sie poza siatka\n";
	}

	cout << "Czas obliczen: "<<(diff / CLOCKS_PER_SEC) << endl;
	copyVectorToPoints(elId, &pointsList);
	clearTempVectors();
	std::cin.get();

	system("pause");

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

void copyVectorToPoints(vector<cl_int> ids, vector<Point>* points){
	for (int i = 0; i < points->size(); ++i){
		(*points)[i].elementId = ids[i];
	}
}

void clearTempVectors(){
	ep1x.clear();
	ep1y.clear();
	ep2x.clear();
	ep2y.clear();
	ep3x.clear();
	ep3y.clear();
	px.clear();
	py.clear();
	elId.clear();
}