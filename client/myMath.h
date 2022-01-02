#pragma once
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
	static int sortByMerge(float data[], int low, int mid, int high);
};

