#include<iostream>
#include<Windows.h>
#include"myMath.h"
#define MAX_THREADS 64
#define SUBDATANUM 4000000
#define DATANUM (SUBDATANUM * MAX_THREADS)
using namespace std;
int main() {
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	float* rawFloatData = new float[DATANUM];
	for (size_t i = 0; i < DATANUM; i++)
	{
		rawFloatData[i] = float(i + 1);
	}
	QueryPerformanceCounter(&start);
	float sum = myMath::sum(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << sum << endl;
	cout << "Time Consumed:" << (end.QuadPart - start.QuadPart) << endl;
	delete[] rawFloatData;
	return 0;
}