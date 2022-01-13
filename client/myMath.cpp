#include "myMath.h"
#include<iostream>
#include<vector>
#include "omp.h"
#include <immintrin.h>
#define MAX_THREADS 64
#define CHUNK_POWER 5
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
	int chunk_num = pow(2, CHUNK_POWER);
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
			int increment = iter * pow(2, k + 1);
			sortByMerge(result, i * increment, (0.5 + i) * increment, (i + 1) * increment);
		}
	}
	return 0;
}

//���������Ƿ���ȷ
int myMath::isVectorSorted(const vector<float>& vfloat) {
	for (int i = 1; i < vfloat.size(); i++) {
		if (vfloat[i] < vfloat[i - 1])
			return -1;
	}
	return 0;
}

int myMath::isSorted(const float data[], const int len) {
	for (int i = 1; i < len; i++) {
		if (data[i] < data[i - 1])
			return -1;
	}
	return 0;
}

//�����������õ����������
vector<float> myMath::mergeSortedArray(float data1[], float data2[]) {
	vector<float> res;
	int i = 0, j = 0;
	while (i < sizeof(data1) / sizeof(data1[0]) && j < sizeof(data2) / sizeof(data2[0])) {
		if (data1[i] < data2[j])
			res.push_back(data1[i++]);
		else
			res.push_back(data2[j++]);
	}
	while (i < sizeof(data1) / sizeof(data1[0]))
		res.push_back(data1[i++]);
	while (j < sizeof(data2) / sizeof(data2[0]))
		res.push_back(data2[j++]);

	return res;
}

vector<float> myMath::mergeSortedArrayAndVec(vector<float>& vfloat, float data[]) {
	vector<float> res;
	int i = 0;
	int j = 0;
	while (i < vfloat.size() && j < sizeof(data) / sizeof(data[0])) {
		if (vfloat[i] < data[j])
			res.push_back(vfloat[i++]);
		else
			res.push_back(data[j++]);
	}
	while (i < vfloat.size())
		res.push_back(vfloat[i++]);
	while (j < sizeof(data) / sizeof(data[0]))
		res.push_back(data[j++]);
	return res;
}

vector<float> myMath::mergeSortedVec(vector<float>& vfloat1, vector<float>& vfloat2) {
	vector<float> res;
	int i = 0, j = 0;
	while (i < vfloat1.size() && j < vfloat2.size()) {
		if (vfloat1[i] < vfloat2[j])
			res.push_back(vfloat1[i++]);
		else
			res.push_back(vfloat2[j++]);
	}
	while (i < vfloat1.size())
		res.push_back(vfloat1[i++]);
	while (j < vfloat2.size())
		res.push_back(vfloat2[j++]);

	return res;
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

//�鲢�����޼���
void myMath::merge(float array[], int low, int mid, int high)
{
	//i ������һ����[low mid]
	int i = low;
	//j �����ڶ�����[mid+1 high]
	int j = mid + 1;
	float* temp = new float[high - low + 1];
	if (!temp) {
		printf("malloc memory failed.\n");
	}
	int count = 0;
	while (i <= mid && j <= high) {
		//���αȽ����������С������Ȼ��װ��temp����
		if (array[i] <= array[j]) {//ȡ�Ⱥ�
			temp[count++] = array[i++];
		}
		else {
			temp[count++] = array[j++];
		}
	}
	//�Ƚ���ɺ󣬼����һ���仹��ʣ�࣬����װ��
	while (i <= mid) {
		temp[count++] = array[i++];
	}
	//�Ƚ���ɺ󣬼����һ���仹��ʣ�࣬����װ��
	while (j <= high) {
		temp[count++] = array[j++];
	}
	//���鲢�ź����Ԫ�ظ�ֵ��ԭ����
	for (int i = low, k = 0; i <= high; i++, k++) {
		array[i] = temp[k];
	}
	delete[]temp;
}
void myMath::cutSortArray(float array[], int low, int high) {
	//�ָ����飬һ��Ϊ2������Ϊ�ģ�����
	int mid = (low + high) / 2;
	if (low < high) {
		cutSortArray(array, low, mid);
		cutSortArray(array, mid + 1, high);
		merge(array, low, mid, high);
	}
}

//���ٹ鲢
void myMath::mergeSpeedUp(int l1, int r1, int r2, float* data, float* temp) {
	int top = l1, p = l1, q = r1;
	while (p < r1 || q < r2) {
		if (q >= r2 || (p < r1 && data[p] <= data[q])) {
			temp[top++] = data[p++];
		}
		else {
			temp[top++] = data[q++];
		}
	}
	for (top = l1; top < r2; top++) {
		data[top] = temp[top];
	}
}
void myMath::merge_sort(int l, int r, float* data, int N) {
	int i, j, t;
	float* temp;
	temp = (float*)malloc(N * sizeof(int));
	//��������һЩ�Ż���Ԥ����ϲ��˵��������䣬��΢��ߵ��ٶ�
#pragma omp parallel for private(i, t) shared(N, data)
	for (i = 0; i < N / 2; i++)
		if (data[i * 2] > data[i * 2 + 1]) {
			t = data[i * 2];
			data[i * 2] = data[i * 2 + 1];
			data[i * 2 + 1] = t;
		}

	//i����ÿ�ι鲢�����䳤�ȣ�j������Ҫ�鲢��������������С���±�
	for (i = 2; i < r; i *= 2) {
#pragma omp parallel for private(j) shared(r, i)
		for (j = 0; j < r - i; j += i * 2) {
			mergeSpeedUp(j, j + i, (j + i * 2 < r ? j + i * 2 : r), data, temp);
		}
	}
}


vector<float> myMath::arrayToVector(float array[]) {
	vector<float> res;
	for (int i = 0; i < sizeof(array) / sizeof(array[0]); i++) {
		res.push_back(array[i]);
	}
	return res;
}