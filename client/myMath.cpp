#include "myMath.h"
#include<iostream>
using namespace std;

float myMath::sum(const float data[], const int len) {
	float sum = 0;
	for (int i = 0; i < len; i++) {
		sum += log(sqrt(data[i]));
	}
	return sum;
}

float myMath::m_max(const float data[], const int len) {
	float min = log(sqrt(data[0]));
	for (int i = 1; i < len; i++) {
		if (log(sqrt(data[i])) < min) {
			min = log(sqrt(data[i]));
		}
	}
	return min;
}

float myMath::sort(const float data[], const int len, float  result[]) {
	return 0;
}

float myMath::sumSpeedUp(const float data[], const int len) {
	return 0;
}

float myMath::m_maxSpeedUp(const float data[], const int len) {
	return 0;
}

float myMath::sortSpeedUp(const float data[], const int len, float  result[]) {
	return 0;
}

