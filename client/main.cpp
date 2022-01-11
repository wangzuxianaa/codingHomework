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
#define SUBDATANUM 20000

#define DATANUM (SUBDATANUM * MAX_THREADS)
using namespace std;
int main() {
	//初始化套接字库
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);//版本号
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "WinSock startup error", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	SOCKADDR_IN addr; //Adres przypisany do socketu Connection
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Addres = localhost
	addr.sin_port = htons(8081); //Port = 1111
	addr.sin_family = AF_INET; //IPv4 Socket

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //Connection
	{
		MessageBoxA(NULL, "Blad Connection", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	cout << "已经连接到服务器端" << endl;
	srand(time(NULL));
	cout << "-------------------------------" << endl;

	LARGE_INTEGER start;
	LARGE_INTEGER end;
	float* rawFloatData = new float[DATANUM];
	for (size_t i = 0; i < DATANUM; i++)
		//rawFloatData[i] = float(i + 1);
		rawFloatData[i] = (rand() % (100 - 1)) + 1;
	//求最大值，无加速
	QueryPerformanceCounter(&start);
	float max0 = myMath::m_max(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求最大值，无加速" << endl;
	cout << "最大值为：" << max0 << endl;
	float timeMax0 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeMax0 << endl;
	cout << "-------------------------------" << endl;

	//求最大值，加速
	QueryPerformanceCounter(&start);
	float max1 = myMath::m_maxSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求最大值，加速" << endl;
	cout << "最大值为：" << max1 << endl;
	float timeMax1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeMax1 << endl;
	
	cout << "求最大值加速比：" << timeMax0 / timeMax1 << endl;
	cout << "-------------------------------" << endl;

	//求和，无加速
	QueryPerformanceCounter(&start);
	float sum0 = myMath::sum(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求和，无加速" << endl;
	cout << "和为：" << sum0 << endl;
	float timeSum0 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSum0 << endl;
	cout << "-------------------------------" << endl;

	//求和，加速
	QueryPerformanceCounter(&start);
	float sum1 = myMath::sumSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求和，加速" << endl;
	cout << "和为：" << sum1 << endl;
	float timeSum1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSum1 << endl;

	cout << "求和加速比：" << timeSum0 / timeSum1 << endl;
	cout << "-------------------------------" << endl;

	//客户端发送数据
	send(Connection, (char*)&max0, sizeof(max0), 0);
	cout << "客户端最大值发送完毕" << endl;
	send(Connection, (char*)&sum0, sizeof(sum0), 0);
	cout << "客户端的和发送完毕" << endl;

	//排序，无加速
	float* result0 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	//myMath::sort(rawFloatData, DATANUM, result0);
	QueryPerformanceCounter(&end);
	cout << "排序正确与否，无加速" <<myMath::isSorted(result0,DATANUM)<< endl;
	float timeSort1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSort1 << endl;
	cout << "-------------------------------" << endl;

	//排序，有加速
	float* result1 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sortSpeedUp(rawFloatData, DATANUM, result1);
	QueryPerformanceCounter(&end);
	cout << "排序正确与否,有加速" << myMath::isSorted(result1, DATANUM)<< endl;
	float timeSort2 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSort2 << endl;

	cout << "排序加速比：" << timeSort1 / timeSort2 << endl;
	cout << "-------------------------------" << endl;
	/*
	for (int i = 0; i < MAX_THREADS; i++) {
		for (int j = 0; j < SUBDATANUM; j++)
		{
			cout << result1[i * SUBDATANUM + j] << "\t";
		}
		cout << "原始" << endl;
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