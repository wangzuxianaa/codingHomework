#include<iostream>
#include<Windows.h>
#include"myMath.h"
#include"omp.h"
#define MAX_THREADS 64
#define SUBDATANUM 2000000
#define DATANUM (SUBDATANUM * MAX_THREADS)
using namespace std;
int main() {
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	float* rawFloatData = new float[DATANUM];
	for (size_t i = 0; i < DATANUM; i++)
		rawFloatData[i] = float(i + 1);

	//求最大值，无加速
	QueryPerformanceCounter(&start);
	float max0 = myMath::m_max(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求最大值，无加速" << endl;
	cout << "最大值为：" << max0 << endl;
	cout << "Time Consumed:" << (end.QuadPart - start.QuadPart) << endl;

	//求最大值，加速
	QueryPerformanceCounter(&start);
	float max1 = myMath::m_maxSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求最大值，加速" << endl;
	cout << "最大值为：" << max1 << endl;
	cout << "Time Consumed:" << (end.QuadPart - start.QuadPart) << endl;

	//求和，无加速
	QueryPerformanceCounter(&start);
	float sum0 = myMath::sum(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求和，无加速" << endl;
	cout << "和为：" << sum0 << endl;
	cout << "Time Consumed:" << (end.QuadPart - start.QuadPart) << endl;

	//求最大值，加速
	QueryPerformanceCounter(&start);
	float sum1 = myMath::sumSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求和，加速" << endl;
	cout << "和为：" << sum1 << endl;
	cout << "Time Consumed:" << (end.QuadPart - start.QuadPart) << endl;

	//排序，无加速
	float* result0 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sort(rawFloatData, DATANUM, result0);
	QueryPerformanceCounter(&end);
	cout << "排序正确与否，无加速" <<myMath::isSorted(result0,DATANUM)<< endl;
	cout << "Time Consumed:" << (end.QuadPart - start.QuadPart) << endl;
	delete[] result0;

	//排序，有加速
	float* result1 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sortSpeedUp(rawFloatData, DATANUM, result1);
	QueryPerformanceCounter(&end);
	cout << "排序正确与否,有加速" << myMath::isSorted(result1, DATANUM) << endl;
	cout << "Time Consumed:" << (end.QuadPart - start.QuadPart) << endl;
	delete[] result1;
	
	delete[] rawFloatData;

	return 0;
}