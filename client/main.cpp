#include<iostream>
#include<Windows.h>
#include"myMath.h"
#include"omp.h"
#include <cstdlib>
#define MAX_THREADS 64
#define SUBDATANUM 1000
#define DATANUM (SUBDATANUM * MAX_THREADS)
using namespace std;
int main() {
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	float* rawFloatData = new float[DATANUM];
	for (size_t i = 0; i < DATANUM; i++)
		//rawFloatData[i] = float(i + 1);
		rawFloatData[i] = (rand() % (100 - 1)) + 1;
	//�����ֵ���޼���
	QueryPerformanceCounter(&start);
	float max0 = myMath::m_max(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "�����ֵ���޼���" << endl;
	cout << "���ֵΪ��" << max0 << endl;
	float timeMax0 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeMax0 << endl;

	//�����ֵ������
	QueryPerformanceCounter(&start);
	float max1 = myMath::m_maxSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "�����ֵ������" << endl;
	cout << "���ֵΪ��" << max1 << endl;
	float timeMax1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeMax1 << endl;
	
	cout << "�����ֵ���ٱȣ�" << timeMax0 / timeMax1 << endl;

	//��ͣ��޼���
	QueryPerformanceCounter(&start);
	float sum0 = myMath::sum(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "��ͣ��޼���" << endl;
	cout << "��Ϊ��" << sum0 << endl;
	float timeSum0 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSum0 << endl;

	//�����ֵ������
	QueryPerformanceCounter(&start);
	float sum1 = myMath::sumSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "��ͣ�����" << endl;
	cout << "��Ϊ��" << sum1 << endl;
	float timeSum1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSum1 << endl;

	cout << "��ͼ��ٱȣ�" << timeSum0 / timeSum1 << endl;

	//�����޼���
	float* result0 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sort(rawFloatData, DATANUM, result0);
	QueryPerformanceCounter(&end);
	cout << "������ȷ����޼���" <<myMath::isSorted(result0,DATANUM)<< endl;
	float timeSort1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSort1 << endl;

	//�����м���
	float* result1 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sortSpeedUp(rawFloatData, DATANUM, result1);
	QueryPerformanceCounter(&end);
	cout << "������ȷ���,�м���" << myMath::isSorted(result1, DATANUM)<< endl;
	float timeSort2 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSort2 << endl;

	cout << "������ٱȣ�" << timeSort1 / timeSort2 << endl;
	/*
	for (int i = 0; i < MAX_THREADS; i++) {
		for (int j = 0; j < SUBDATANUM; j++)
		{
			cout << result1[i * SUBDATANUM + j] << "\t";
		}
		cout << "ԭʼ" << endl;
		for (int j = 0; j < SUBDATANUM; j++)
		{
			cout << rawFloatData[i * SUBDATANUM + j] << "\t";
		}
		cout << "\n" << endl;
	}*/
	delete[] result0;
	delete[] result1;
	delete[] rawFloatData;


	
	return 0;
}