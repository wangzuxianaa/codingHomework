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
	cout << "Time Consumed:" << timeMax0*1000/freq.QuadPart << "ms" << endl;//单位为ms
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
	cout << "Time Consumed: " << timeSum0*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//求最大值，加速
	QueryPerformanceCounter(&start);
	float sum1 = myMath::sumSpeedUp(rawFloatData, DATANUM);
	QueryPerformanceCounter(&end);
	cout << "求和，加速" << endl;
	cout << "和为：" << sum1 << endl;
	float timeSum1 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSum1*1000/freq.QuadPart << "ms" << endl;

	cout << "求和加速比：" << timeSum0 / timeSum1 << endl;
	cout << "-------------------------------" << endl;

	//接受客户端传输的最大值
	float max_recv;
	float sum_recv;
	QueryPerformanceCounter(&start);
	int maxRecvRes = recv(newConnection, (char*)&max_recv, sizeof(max_recv), 0);
	while (TRUE) {
		if (maxRecvRes > 0) {
			cout << "服务端接受最大值数据成功" << endl;
			break;
		}
		else
			cout << "服务端接受最大值数据失败" << endl;
	}
	QueryPerformanceCounter(&end);
	float timeMaxTransmit = end.QuadPart - start.QuadPart;
	cout << "接受到的客户端的最大值是: " << max_recv << endl;
	cout << "所以总的最大值是: " << max(max0, max_recv) << endl;
	cout << "接受最大值的传输时间为: " << timeMaxTransmit*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//接受客户端传输的总和
	QueryPerformanceCounter(&start);
	int sumRecvRes = recv(newConnection, (char*)&sum_recv, sizeof(sum_recv), 0);
	while (TRUE) {
		if (sumRecvRes > 0) {
			cout << "服务端接受和数据成功" << endl;
			break;
		}
		else
			cout << "服务端接受和数据失败" << endl;
	}
	QueryPerformanceCounter(&end);
	float timeSumTransmit = end.QuadPart - start.QuadPart;
	cout << "接受到的客户端的和是: " << sum_recv << endl;
	cout << "所以总的和是: " << sum_recv + sum0 << endl;
	cout << "接受总和的传输时间为: " << timeSumTransmit*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;


	//排序，无加速
	float* result0 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sort(rawFloatData, DATANUM, result0);
	QueryPerformanceCounter(&end);
	float timeSort1 = end.QuadPart - start.QuadPart;
	if (myMath::isSorted(result0, DATANUM) == 0)
		cout << "排序正确(无加速)" << endl;
	else
		cout << "排序错误(无加速)" << endl;
	cout << "Time Consumed: " << timeSort1*1000/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//排序，有加速
	float* result1 = new float[DATANUM];
	QueryPerformanceCounter(&start);
	myMath::sortSpeedUp(rawFloatData, DATANUM, result1);
	QueryPerformanceCounter(&end);
	if (myMath::isSorted(result1, DATANUM) == 0)
		cout << "排序正确(有加速)" << endl;
	else
		cout << "排序错误(有加速)" << endl;
	float timeSort2 = end.QuadPart - start.QuadPart;
	cout << "Time Consumed: " << timeSort2*1000/freq.QuadPart << "ms" << endl;

	cout << "排序加速比：" << timeSort1 / timeSort2 << endl;
	cout << "-------------------------------" << endl;

	//客户端排序结果传输
	cout << "开始接受客户端数据" << endl;
	float* result1_recv = new float[DATANUM];
	QueryPerformanceCounter(&start);
	int sortRecvRes = recv(newConnection, (char*)result1_recv, sizeof(result1_recv), 0);
	while (TRUE) {
		if (sortRecvRes > 0) {
			cout << "服务端接受排序数据成功" << endl;
			break;
		}
		else
			cout << "服务端接受排序数据失败" << endl;
	}
	QueryPerformanceCounter(&end);
	float timeSortSpeedUpTransmit = end.QuadPart - start.QuadPart;
	cout << "接受客户端排序结果成功，并存入result1_recv" << endl;
	cout << "接受客户端数据所花费的时间是:" << 1000*timeSortSpeedUpTransmit/freq.QuadPart << "ms" << endl;
	cout << "-------------------------------" << endl;

	//对服务端程序和传输的客户端程序进行归并排序
	QueryPerformanceCounter(&start);
	vector<float> res = myMath::mergeSortedArray(result1, result1_recv);
	QueryPerformanceCounter(&end);
	float timeMergeSortedArray = end.QuadPart - start.QuadPart;
	while (TRUE) {
		if (myMath::isVectorSorted(res) == 0) {
			cout << "两组数据排序正确" << endl;
			break;
		}
		else {
			cout << "两组数据排序错误" << endl;
		}
	}
	cout << "两组数据归并排序所花费的时间: " << timeMergeSortedArray*1000/freq.QuadPart << "ms" << endl;


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
	delete[] result1_recv;


	closesocket(newConnection);
	WSACleanup();
	return 0;
}