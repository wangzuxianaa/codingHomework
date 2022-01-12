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
#define SUBDATANUM 10000
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
	cout << "Time Consumed:" << timeMax0*1000/freq.QuadPart << "ms" << endl;//��λΪms
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
	cout << "Time Consumed: " << timeSum0*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//�����ֵ������
	QueryPerformanceCounter(&start);
	float sum1 = myMath::sumSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "��ͣ�����" << endl;
	cout << "��Ϊ��" << sum1 << endl;
	float timeSum1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSum1*1000/freq.QuadPart << "ms" << endl;

	cout << "��ͼ��ٱȣ�" << timeSum0 / timeSum1 << endl;
	cout << "-------------------------------" << endl;

	//���ܿͻ��˴�������ֵ
	float max_recv;
	float sum_recv;
	QueryPerformanceCounter(&start);
	int maxRecvRes = recv(newConnection, (char*)&max_recv, sizeof(max_recv), 0);
	while (TRUE) {
		if (maxRecvRes > 0) {
			cout << "����˽������ֵ���ݳɹ�" << endl;
			break;
		}
		else
			cout << "����˽������ֵ����ʧ��" << endl;
	}
	QueryPerformanceCounter(&end);
	float timeMaxTransmit = end.QuadPart - start.QuadPart;
	cout << "���ܵ��Ŀͻ��˵����ֵ��: " << max_recv << endl;
	cout << "�����ܵ����ֵ��: " << max(max0, max_recv) << endl;
	cout << "�������ֵ�Ĵ���ʱ��Ϊ: " << timeMaxTransmit*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//���ܿͻ��˴�����ܺ�
	QueryPerformanceCounter(&start);
	int sumRecvRes = recv(newConnection, (char*)&sum_recv, sizeof(sum_recv), 0);
	while (TRUE) {
		if (sumRecvRes > 0) {
			cout << "����˽��ܺ����ݳɹ�" << endl;
			break;
		}
		else
			cout << "����˽��ܺ�����ʧ��" << endl;
	}
	QueryPerformanceCounter(&end);
	float timeSumTransmit = end.QuadPart - start.QuadPart;
	cout << "���ܵ��Ŀͻ��˵ĺ���: " << sum_recv << endl;
	cout << "�����ܵĺ���: " << sum_recv + sum0 << endl;
	cout << "�����ܺ͵Ĵ���ʱ��Ϊ: " << timeSumTransmit*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;


	//�����޼���
	float* result0 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sort(rawFloatData, DATANUM, result0);
	QueryPerformanceCounter(&end);
	float timeSort1 = end.QuadPart - start.QuadPart;
	if (myMath::isSorted(result0, DATANUM) == 0)
		cout << "������ȷ(�޼���)" << endl;
	else
		cout << "�������(�޼���)" << endl;
	cout << "Time Consumed: " << timeSort1*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//�����м���
	float* result1 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sortSpeedUp(rawFloatData, DATANUM, result1);
	QueryPerformanceCounter(&end);
	if (myMath::isSorted(result1, DATANUM) == 0)
		cout << "������ȷ(�м���)" << endl;
	else
		cout << "�������(�м���)" << endl;
	float timeSort2 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSort2*1000/freq.QuadPart << "ms" << endl;

	cout << "������ٱȣ�" << timeSort1 / timeSort2 << endl;
	cout << "-------------------------------" << endl;

	//�ͻ�������������
	cout << "��ʼ���ܿͻ�������" << endl;
	float* result1_recv = new float[DATANUM];
	QueryPerformanceCounter(&start);
	int sortRecvRes = recv(newConnection, (char*)result1_recv, sizeof(result1_recv), 0);
	while (TRUE) {
		if (sortRecvRes > 0) {
			cout << "����˽����������ݳɹ�" << endl;
			break;
		}
		else
			cout << "����˽�����������ʧ��" << endl;
	}
	QueryPerformanceCounter(&end);
	float timeSortSpeedUpTransmit = end.QuadPart - start.QuadPart;
	cout << "���ܿͻ����������ɹ���������result1_recv" << endl;
	cout << "���ܿͻ������������ѵ�ʱ����:" << 1000*timeSortSpeedUpTransmit/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//�Է���˳���ʹ���Ŀͻ��˳�����й鲢����
	QueryPerformanceCounter(&start);
	vector<float> res = myMath::mergeSortedArray(result1, result1_recv);
	QueryPerformanceCounter(&end);
	float timeMergeSortedArray = end.QuadPart - start.QuadPart;
	while (TRUE) {
		if (myMath::isVectorSorted(res) == 0) {
			cout << "��������������ȷ" << endl;
			break;
		}
		else {
			cout << "���������������" << endl;
		}
	}
	cout << "�������ݹ鲢���������ѵ�ʱ��: " << timeMergeSortedArray*1000/freq.QuadPart << "ms" << endl;


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
	delete[] result1_recv;


	closesocket(newConnection);
	WSACleanup();
	return 0;
}