#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <Windows.h>
#include "myMath.h"
#include "omp.h"
#include <cstdlib>
#include <stdlib.h>
#include <time.h>

#define MAX_THREADS 64
#define SUBDATANUM 1000000

#define DATANUM (SUBDATANUM * MAX_THREADS)
using namespace std;
int main() {
	//��ʼ���׽��ֿ�
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);//�汾��
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "WinSock startup error", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	SOCKADDR_IN addr; //Adres przypisany do socketu Connection
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Addres = localhost
	addr.sin_port = htons(8080); //Port = 1111
	addr.sin_family = AF_INET; //IPv4 Socket

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //Connection
	{
		MessageBoxA(NULL, "Blad Connection", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	cout << "�Ѿ����ӵ���������" << endl;
	srand(time(NULL));
	cout << "-------------------------------" << endl;

	LARGE_INTEGER start;
	LARGE_INTEGER end;
	LARGE_INTEGER freq;
	float* rawFloatData = new float[DATANUM];
	for (size_t i = 0; i < DATANUM; i++)
		//rawFloatData[i] = float(i + 1);
		rawFloatData[i] = (rand() % (100 - 1)) + 1;
	//�����ֵ���޼���
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	float max0 = myMath::m_max(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "�����ֵ���޼���" << endl;
	cout << "���ֵΪ��" << max0 << endl;
	float timeMax0 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeMax0*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//�����ֵ������
	QueryPerformanceCounter(&start);
	float max1 = myMath::m_maxSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "�����ֵ������" << endl;
	cout << "���ֵΪ��" << max1 << endl;
	float timeMax1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeMax1*1000/freq.QuadPart << "ms" << endl;
	
	cout << "�����ֵ���ٱȣ�" << timeMax0 / timeMax1 << endl;
	cout << "-------------------------------" << endl;

	//��ͣ��޼���
	QueryPerformanceCounter(&start);
	float sum0 = myMath::sum(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "��ͣ��޼���" << endl;
	cout << "��Ϊ��" << sum0 << endl;
	float timeSum0 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSum0 * 1000 / freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//��ͣ�����
	QueryPerformanceCounter(&start);
	float sum1 = myMath::sumSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "��ͣ�����" << endl;
	cout << "��Ϊ��" << sum1 << endl;
	float timeSum1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSum1 * 1000 / freq.QuadPart << "ms" << endl;

	cout << "��ͼ��ٱȣ�" << timeSum0 / timeSum1 << endl;
	cout << "-------------------------------" << endl;

	//�ͻ��˷������ֵ����
	int maxSendRes = send(Connection, (char*)&max0, sizeof(max0), 0);
	while (TRUE) {
		if (maxSendRes > 0) {
			cout << "�ͻ������ֵ�������" << endl;
			break;
		}
		else
			cout << "�ͻ������ֵ����ʧ��" << endl;
	}
	cout << "-------------------------------" << endl;

	//�ͻ��˷��ͺ͵�����
	int sumSendRes = send(Connection, (char*)&sum0, sizeof(sum0), 0);
	while (TRUE) {
		if (sumSendRes > 0) {
			cout << "�ͻ��˺ͷ������" << endl;
			break;
		}
		else
			cout << "�ͻ��˺ͷ���ʧ��" << endl;
	}
	cout << "-------------------------------" << endl;

	//�����޼���
	float* result0 = new float[DATANUM];
	for (int i = 0; i < DATANUM; i++) {
		result0[i] = rawFloatData[i];
	}
	QueryPerformanceCounter(&start);
	myMath::cutSortArray(result0, 0, DATANUM - 1);
	//myMath::sort(rawFloatData, DATANUM, result0);
	QueryPerformanceCounter(&end);
	if (myMath::isSorted(result0, DATANUM) == 0)
		cout << "������ȷ(�޼���)" << endl;
	else
		cout << "�������(�޼���)" << endl;
	float timeSort1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSort1*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//�����м���
	float* result1 = new float[DATANUM];
	for (int i = 0; i < DATANUM; i++) {
		result1[i] = rawFloatData[i];
	}
	QueryPerformanceCounter(&start);
	//myMath::sortSpeedUp(rawFloatData, DATANUM, result1);
	myMath::merge_sort(0, DATANUM, result1, DATANUM);
	QueryPerformanceCounter(&end);
	if (myMath::isSorted(result1,DATANUM) == 0)
		cout << "������ȷ(�м���)" << endl;
	else
		cout << "�������(�м���)" << endl;
	float timeSort2 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSort2 * 1000 / freq.QuadPart << "ms" << endl;

	cout << "������ٱȣ�" << timeSort1 / timeSort2 << endl;
	cout << "-------------------------------" << endl;

	//������������鴫�������
	int sortSendRes = send(Connection, (char*)result1, sizeof(result1), 0);
	while (TRUE) {
		if (sortSendRes > 0) {
			cout << "�ͻ������������" << endl;
			break;
		}
		else
			cout << "�ͻ���������ʧ��" << endl;
	}
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
	//delete[] result0;
	delete[] result1;
	delete[] rawFloatData;

	closesocket(Connection);
	WSACleanup();

	
	return 0;
}