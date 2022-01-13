#pragma once
#include <vector>

using namespace std;
class myMath
{
public:
	static float sum(const float data[], const int len);
	static float m_max(const float data[], const int len);
	static float sort(const float data[], const int len, float  result[]);
	static float sumSpeedUp(const float data[], const int len);
	static float m_maxSpeedUp(const float data[], const int len);
	static float sortSpeedUp(const float data[], const int len, float  result[]);
	static int isSorted(const float data[], const int len);
	//合并有序数组
	static vector<float> mergeSortedArray(float data1[], float data2[]);
	static vector<float> mergeSortedArrayAndVec(vector<float>& vfloat, float data[]);
	static vector<float> mergeSortedVec(vector<float>& vfloat1, vector<float>& vfloat2);
	static int isVectorSorted(const vector<float>& vfloat);
	static int sortByMerge(float data[], int low, int mid, int high);
	static void merge(float array[], int low, int mid, int high);
	static void cutSortArray(float array[], int low, int high);
	//归并加速
	static void mergeSpeedUp(int l1, int r1, int r2, float* data, float* temp);
	static void merge_sort(int l, int r, float* data, int N);

	static vector<float> arrayToVector(float array[]);//数组转容器
};