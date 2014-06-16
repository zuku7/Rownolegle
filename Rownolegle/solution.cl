/**
* Oblicza wartosc potrzebna do sprawdzenia czy punkt znajduje sie w trojkacie.
* @param p1x-p3y - tablice float z wspolrzednymi punktow trojkata.
*/
float mSign(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y){
	float sol = (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
	return sol;
}

/**
* Sprawdza czy punkt znajduje sie w trojkacie.
* @param ptx-pty - tablice float z wspolrzednymi sprawdzanego punktu.
* @param v1x-v3y - tablice float z wspolrzednymi punktow elementu.
*/
bool PointInTriangle(float ptx, float pty, float v1x, float v1y, float v2x, float v2y, float v3x, float v3y){
	bool b1, b2, b3;

	b1 = mSign(ptx,pty, v1x,v1y, v2x,v2y) < 0.0f;
	b2 = mSign(ptx, pty, v2x, v2y, v3x, v3y) < 0.0f;
	b3 = mSign(ptx,pty, v3x,v3y, v1x,v1y) < 0.0f;

	return ((b1 == b2) && (b2 == b3));
}

/**
* Glowna funkcja kernela.
* @param bP1x-bP3y - tablice float z wspolrzednymi punktow elementow.
* @param bESize - ilosc elementow.
* @param bPx-bPy - tablice float z wspolrzednymi sprawdzanych punktow.
* @param bPSize - ilosc sprawdzanych punktow.
* @param outElId - tablica do zapisu zwroconych wynikow.
*/
__kernel void multiply(__global float* bP1x, __global float* bP1y, __global float* bP2x, __global float* bP2y, __global float* bP3x, __global float* bP3y, int bESize,
	__global float* bPx, __global float* bPy, int bPSize, __global int* outElId)
{
	size_t id = get_global_id(0);
	if (id < bPSize)
	{
		outElId[id] = -1;
		//for (int i = 0; i < bPSize; ++i){
		for (int j = 0; j < bESize; ++j){
			//for (Element e : elementsList){
			if (PointInTriangle(bPx[id], bPy[id], bP1x[j], bP1y[j], bP2x[j], bP2y[j], bP3x[j], bP3y[j])){
				outElId[id] = j;
				break;
			}
		}
		//}

		//outElId[id] = 2;
	}
}
