#include<iostream>
#include<Windows.h>
#include"myMath.h"
#include"omp.h"
#define MAX_THREADS 64
#define SUBDATANUM 4000000
#define DATANUM (SUBDATANUM * MAX_THREADS)
using namespace std;
int main() {
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	float* rawFloatData = new float[DATANUM];
	for (size_t i = 0; i < DATANUM; i++)
		rawFloatData[i] = float(i + 1);
	QueryPerformanceCounter(&start);
	float maxNum = myMath::m_maxSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << maxNum << endl;
	cout << "Time Consumed:" << (end.QuadPart - start.QuadPart) << endl;
	delete[] rawFloatData;
	return 0;
}