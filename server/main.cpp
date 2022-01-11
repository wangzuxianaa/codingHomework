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
	//初始化套接字库
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);//版本号
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "WinSock startup error", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	//绑定IP地址，端口号和协议簇
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //target PC
	addr.sin_port = htons(8081); // sever Port
	addr.sin_family = AF_INET; //IPv4 Socket

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));

	//进入监听状态
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);
	if (newConnection == 0)
	{
		cout << "连接失败" << endl;
	}
	else
		cout << "连接建立,可以接受数据" << endl;
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

	/*float max_recv;

	int recv_len = recv(newConnection,(char*)&max_recv,sizeof(max_recv),0);
	cout << "接受到的客户端的最大值是:" << max_recv << endl;*/

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

	//求最大值，加速
	QueryPerformanceCounter(&start);
	float sum1 = myMath::sumSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求和，加速" << endl;
	cout << "和为：" << sum1 << endl;
	float timeSum1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSum1 << endl;

	cout << "求和加速比：" << timeSum0 / timeSum1 << endl;
	cout << "-------------------------------" << endl;

	//接受客户端传输的最大值和总和数据
	float max_recv;
	float sum_recv;
	recv(newConnection, (char*)&max_recv, sizeof(max_recv), 0);
	cout << "接受到的客户端的最大值是: " << max_recv << endl;
	cout << "所以总的最大值是: " << max(max0, max_recv) << endl;
	recv(newConnection, (char*)&sum_recv, sizeof(sum_recv), 0);
	cout << "接受到的客户端的和是：" << sum_recv << endl;
	cout << "所以总的和是: " << sum_recv + sum0 << endl;

	//排序，无加速
	float* result0 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sort(rawFloatData, DATANUM, result0);
	QueryPerformanceCounter(&end);
	cout << "排序正确与否，无加速:" << myMath::isSorted(result0, DATANUM) << "若是0，则排序正确，错误为-1" << endl;
	float timeSort1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSort1 << endl;
	cout << "-------------------------------" << endl;

	//排序，有加速
	float* result1 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sortSpeedUp(rawFloatData, DATANUM, result1);
	QueryPerformanceCounter(&end);
	cout << "排序正确与否,有加速" << myMath::isSorted(result1, DATANUM) << "若是0，则排序正确，错误为-1" << endl;
	float timeSort2 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed:" << timeSort2 << endl;

	cout << "排序加速比：" << timeSort1 / timeSort2 << endl;

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


	closesocket(newConnection);
	WSACleanup();
	return 0;
}