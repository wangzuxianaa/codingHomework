#include "myMath.h"
#include<iostream>
#include "omp.h"
#include <immintrin.h>
#define MAX_THREADS 64
#define CHUNK_POWER 9
using namespace std;

float myMath::sum(const float data[], const int len) {
	//���sumNum��float�ͣ������2^23�����ޣ�Ϊɶ
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

//ð������,����ʹ��log��sqrt())����ԭʼ����
//��С��������
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
	int iter = len / (MAX_THREADS * 8);  //_m256�洢8λ������
	__m256* tempResult = new __m256[MAX_THREADS]; //�����ݷ�ΪMAX_THREADS��
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

	int iter = len / (MAX_THREADS * 8);  //_m256���Դ洢8λ������
	__m256* tempResult = new __m256[MAX_THREADS]; //�����ݷ�ΪMAX_THREADS��
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
	int chunk_num = pow(2,CHUNK_POWER);
	int iter = len / chunk_num;  //��ΪMAX_THREADS�飬ÿ����iter��ֵ
#pragma omp parallel for num_threads(chunk_num)
	//��ÿ�����ð������
	for (int k = 0; k < chunk_num; k++) {
		int x = k * iter;
		for (int i = 0; i < iter; i++) {
			for (int j = 0; j < iter - 1 - i; j++) {
				if (result[x + j] > result[x + j + 1]) {
					float temp = result[x + j];
					result[x + j] = result[x + j + 1];
					result[x + j + 1] = temp;
				}
			}
		}
	}
	for (int k = 0; k < log(chunk_num) / log(2); k++) {
		int thread_nums = chunk_num / (pow(2, k + 1));
#pragma omp parallel for num_threads(thread_nums)
		for (int i = 0; i < thread_nums; i++) {
			int increment = iter * pow(2, k+1);
			sortByMerge(result, i * increment, (0.5+i) * increment,(i+1) *increment);
		}
	}
	return 0;
}

//���������Ƿ���ȷ
int myMath::isSorted(const float data[], const int len) {
	for (int i = 1; i < len; i++) {
		if (data[i] < data[i - 1])
			return -1;
	}
	return 0;
}

//�鲢���򣬴�С����
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