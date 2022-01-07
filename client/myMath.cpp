#include "myMath.h"
#include<iostream>
#include "omp.h"
#include <immintrin.h>
#define MAX_THREADS 64
using namespace std;

float myMath::sum(const float data[], const int len) {
	//如果sumNum是float型，会存在2^23的上限？为啥
	double sumNum = 0;
	for (size_t i = 0; i < len; i++) {
		sumNum += log(sqrt(data[i]));
	}
	return sumNum;
}

float myMath::m_max(const float data[], const int len) {
	float maxNum = log(sqrt(data[0]));
	for (int i = 1; i < len; i++) {
		float temp = log(sqrt(data[i]));
		if (temp > maxNum) {
			maxNum = temp;
		}
	}
	return maxNum;
}

//冒泡排序,不再使用log（sqrt())访问原始数据
//从小到大排序
float myMath::sort(const float data[], const int len, float  result[]) {
	for (int i = 0; i < len; i++) {
		result[i] = data[i];
	}
	for (int i = 0; i < len - 1; i++) {
		for (int j = 0; j < len - 1 - i; j++) {
			if (result[j] > result[j + 1]) {
				float temp = result[j + 1];
				result[j + 1] = result[j];
				result[j] = temp;
			}
		}
	}
	return 0;
}

float myMath::sumSpeedUp(const float data[], const int len) {
	float sumNum = 0;
	int iter = len / (MAX_THREADS * 8);  //_m256存储8位浮点数
	__m256* tempResult = new __m256[MAX_THREADS]; //将数据分为MAX_THREADS块
#pragma omp parallel for num_threads(MAX_THREADS)
	for (int i = 0; i < MAX_THREADS; i++) {
		tempResult[i] = _mm256_setzero_ps();
		__m256* ptr = (__m256*)(data + i * (len / MAX_THREADS));
		for (int j = 0; j < iter; j++, ptr++) {
			tempResult[i] = _mm256_add_ps(tempResult[i], _mm256_log_ps(_mm256_sqrt_ps(*ptr)));
		}
	}
	__m256 localResult = _mm256_setzero_ps();
	for (int i = 0; i < MAX_THREADS; i++) {
		localResult = _mm256_add_ps(localResult, tempResult[i]);
	}
	float* floatResult = (float*)&localResult;
	for (int i = 0; i < 8; i++) {
		sumNum = sumNum + floatResult[i];
	}
	delete[] tempResult;
	return sumNum;
}

float myMath::m_maxSpeedUp(const float data[], const int len) {
	float maxNum = log(sqrt(data[0]));

	int iter = len / (MAX_THREADS * 8);  //_m256可以存储8位浮点数
	__m256* tempResult = new __m256[MAX_THREADS]; //将数据分为MAX_THREADS块
#pragma omp parallel for num_threads(MAX_THREADS)
	for (int i = 0; i < MAX_THREADS; i++) {
		tempResult[i] = _mm256_setzero_ps();
		__m256* ptr = (__m256*)(data + i * (len / MAX_THREADS));
		for (int j = 0; j < iter; j++, ptr++) {
			tempResult[i] = _mm256_max_ps(tempResult[i], _mm256_log_ps(_mm256_sqrt_ps(*ptr)));
		}
	}
	__m256 localResult = _mm256_setzero_ps();
	for (int i = 0; i < MAX_THREADS; i++) {
		localResult = _mm256_max_ps(localResult, tempResult[i]);
	}
	float* floatResult = (float*)&localResult;
	for (int i = 0; i < 8; i++) {
		if (maxNum < floatResult[i])
			maxNum = floatResult[i];
	}
	delete[] tempResult;
	return maxNum;
}

float myMath::sortSpeedUp(const float data[], const int len, float  result[]) {
	for (int i = 0; i < len; i++) {
		result[i] = data[i];
	}
	int iter = len / MAX_THREADS;  //分为MAX_THREADS块，每块有iter个值
#pragma omp parallel for num_threads(MAX_THREADS)
	//对每块进行冒泡排序
	for (int i = 0; i < MAX_THREADS; i++) {
		for (int j = 0; j < iter; j++) {
			for (int k = 0; k < iter - i - 1; k++) {
				if (result[i * iter + k] > result[i * iter + k + 1]) {
					float temp = result[i * iter + k + 1];
					result[i * iter + k + 1] = result[i * iter + k];
					result[i * iter + k] = temp;
				}
			}
		}
	}
	//归并排序
	for (int round = 0; round < (log(MAX_THREADS / log(2))); round++) {
		int dataNum = 2*pow(2,round) * iter;   //第n轮归并
		int threadNum = MAX_THREADS / (pow(2, round + 1));//需要的线程数
#pragma omp parallel for num_threads(MAX_THREADS)
		for (int i = 0; i < threadNum; i++) {
			int low = dataNum * threadNum;
			int mid = dataNum * (threadNum+0.5);
			int high = dataNum * (threadNum+1);
			sortByMerge(result, low, mid, high);
		}
	}
	return 0;
}

//
int myMath::isSorted(const float data[], const int len) {
	for (int i = 1; i < len; i++) {
		if (data[i] < data[i - 1])
			return -1;
	}
	return 0;
}

int myMath::sortByMerge(float data[], int low, int mid, int high) {
	float* temp = new float[high - low];
	int count = 0;
	int i = low;
	int j = mid;
	for (i = low, j = mid; i < mid && j < high;) {
		if (data[i] <= data[j]) {
			temp[count++] = data[i];
			i++;
		}
		else {
			temp[count++] = data[j];
			j++;
		}
	}
	if (i < mid) {
		for (; i < mid; i++) {
			temp[count++] = data[i];
		}
	}
	else {
		for (; j < high; j++) {
			temp[count++] = data[j];
		}
	}
	for (int k = 0, i = low; k < high - low; k++, i++) {
		data[i] = temp[k];
	}
	delete[] temp;
	return 0;
}