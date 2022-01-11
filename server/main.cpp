#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#pragma comment(lib,"ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include "myMath.h"
#include "omp.h"
#include <cstdlib>

#define MAX_THREADS 64
#define SUBDATANUM 20000
#define DATANUM (SUBDATANUM * MAX_THREADS)

char maxMessage[1024];
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

	//��IP��ַ���˿ںź�Э���
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //target PC
	addr.sin_port = htons(8081); // sever Port
	addr.sin_family = AF_INET; //IPv4 Socket

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));

	//�������״̬
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);
	if (newConnection == 0)
	{
		cout << "����ʧ��" << endl;
	}
	else
		cout << "���ӽ���,���Խ�������" << endl;
	cout << "-------------------------------" << endl;

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
	cout << "-------------------------------" << endl;

	/*float max_recv;

	int recv_len = recv(newConnection,(char*)&max_recv,sizeof(max_recv),0);
	cout << "���ܵ��Ŀͻ��˵����ֵ��:" << max_recv << endl;*/

	//�����ֵ������
	QueryPerformanceCounter(&start);
	float max1 = myMath::m_maxSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "�����ֵ������" << endl;
	cout << "���ֵΪ��" << max1 << endl;
	float timeMax1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeMax1 << endl;

	cout << "�����ֵ���ٱȣ�" << timeMax0 / timeMax1 << endl;
	cout << "-------------------------------" << endl;

	//��ͣ��޼���
	QueryPerformanceCounter(&start);
	float sum0 = myMath::sum(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "��ͣ��޼���" << endl;
	cout << "��Ϊ��" << sum0 << endl;
	float timeSum0 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSum0 << endl;
	cout << "-------------------------------" << endl;

	//�����ֵ������
	QueryPerformanceCounter(&start);
	float sum1 = myMath::sumSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "��ͣ�����" << endl;
	cout << "��Ϊ��" << sum1 << endl;
	float timeSum1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSum1 << endl;

	cout << "��ͼ��ٱȣ�" << timeSum0 / timeSum1 << endl;
	cout << "-------------------------------" << endl;

	//���ܿͻ��˴�������ֵ���ܺ�����
	float max_recv;
	float sum_recv;
	recv(newConnection, (char*)&max_recv, sizeof(max_recv), 0);
	cout << "���ܵ��Ŀͻ��˵����ֵ��: " << max_recv << endl;
	cout << "�����ܵ����ֵ��: " << max(max0, max_recv) << endl;
	recv(newConnection, (char*)&sum_recv, sizeof(sum_recv), 0);
	cout << "���ܵ��Ŀͻ��˵ĺ��ǣ�" << sum_recv << endl;
	cout << "�����ܵĺ���: " << sum_recv + sum0 << endl;

	//�����޼���
	float* result0 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sort(rawFloatData, DATANUM, result0);
	QueryPerformanceCounter(&end);
	cout << "������ȷ����޼���:" << myMath::isSorted(result0, DATANUM) << "����0����������ȷ������Ϊ-1" << endl;
	float timeSort1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSort1 << endl;
	cout << "-------------------------------" << endl;

	//�����м���
	float* result1 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sortSpeedUp(rawFloatData, DATANUM, result1);
	QueryPerformanceCounter(&end);
	cout << "������ȷ���,�м���" << myMath::isSorted(result1, DATANUM) << "����0����������ȷ������Ϊ-1" << endl;
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


	closesocket(newConnection);
	WSACleanup();
	return 0;
}