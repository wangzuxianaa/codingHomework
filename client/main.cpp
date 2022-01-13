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
	addr.sin_port = htons(8080); //Port = 1111
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
	LARGE_INTEGER freq;
	float* rawFloatData = new float[DATANUM];
	for (size_t i = 0; i < DATANUM; i++)
		//rawFloatData[i] = float(i + 1);
		rawFloatData[i] = (rand() % (100 - 1)) + 1;
	//求最大值，无加速
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	float max0 = myMath::m_max(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求最大值，无加速" << endl;
	cout << "最大值为：" << max0 << endl;
	float timeMax0 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeMax0*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//求最大值，加速
	QueryPerformanceCounter(&start);
	float max1 = myMath::m_maxSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求最大值，加速" << endl;
	cout << "最大值为：" << max1 << endl;
	float timeMax1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeMax1*1000/freq.QuadPart << "ms" << endl;
	
	cout << "求最大值加速比：" << timeMax0 / timeMax1 << endl;
	cout << "-------------------------------" << endl;

	//求和，无加速
	QueryPerformanceCounter(&start);
	float sum0 = myMath::sum(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求和，无加速" << endl;
	cout << "和为：" << sum0 << endl;
	float timeSum0 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSum0 * 1000 / freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//求和，加速
	QueryPerformanceCounter(&start);
	float sum1 = myMath::sumSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求和，加速" << endl;
	cout << "和为：" << sum1 << endl;
	float timeSum1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSum1 * 1000 / freq.QuadPart << "ms" << endl;

	cout << "求和加速比：" << timeSum0 / timeSum1 << endl;
	cout << "-------------------------------" << endl;

	//客户端发送最大值数据
	int maxSendRes = send(Connection, (char*)&max0, sizeof(max0), 0);
	while (TRUE) {
		if (maxSendRes > 0) {
			cout << "客户端最大值发送完毕" << endl;
			break;
		}
		else
			cout << "客户端最大值发送失败" << endl;
	}
	cout << "-------------------------------" << endl;

	//客户端发送和的数据
	int sumSendRes = send(Connection, (char*)&sum0, sizeof(sum0), 0);
	while (TRUE) {
		if (sumSendRes > 0) {
			cout << "客户端和发送完毕" << endl;
			break;
		}
		else
			cout << "客户端和发送失败" << endl;
	}
	cout << "-------------------------------" << endl;

	//排序，无加速
	float* result0 = new float[DATANUM];
	for (int i = 0; i < DATANUM; i++) {
		result0[i] = rawFloatData[i];
	}
	QueryPerformanceCounter(&start);
	myMath::cutSortArray(result0, 0, DATANUM - 1);
	//myMath::sort(rawFloatData, DATANUM, result0);
	QueryPerformanceCounter(&end);
	if (myMath::isSorted(result0, DATANUM) == 0)
		cout << "排序正确(无加速)" << endl;
	else
		cout << "排序错误(无加速)" << endl;
	float timeSort1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSort1*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//排序，有加速
	float* result1 = new float[DATANUM];
	for (int i = 0; i < DATANUM; i++) {
		result1[i] = rawFloatData[i];
	}
	QueryPerformanceCounter(&start);
	//myMath::sortSpeedUp(rawFloatData, DATANUM, result1);
	myMath::merge_sort(0, DATANUM, result1, DATANUM);
	QueryPerformanceCounter(&end);
	if (myMath::isSorted(result1,DATANUM) == 0)
		cout << "排序正确(有加速)" << endl;
	else
		cout << "排序错误(有加速)" << endl;
	float timeSort2 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSort2 * 1000 / freq.QuadPart << "ms" << endl;

	cout << "排序加速比：" << timeSort1 / timeSort2 << endl;
	cout << "-------------------------------" << endl;

	//将加速完的数组传给服务端
	int sortSendRes = send(Connection, (char*)result1, sizeof(result1), 0);
	while (TRUE) {
		if (sortSendRes > 0) {
			cout << "客户端排序发送完毕" << endl;
			break;
		}
		else
			cout << "客户端排序发送失败" << endl;
	}
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
	//delete[] result0;
	delete[] result1;
	delete[] rawFloatData;

	closesocket(Connection);
	WSACleanup();

	
	return 0;
}