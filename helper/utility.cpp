#include "utility.h"
#include <cmath>

bool InRange(const int value, const int min, const int max)
{
	return value < max && value > min;
}

bool ColorEqual(const Color& c1, const Color& c2) {
	return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

void MoveTowards(float& src, float target, float max)
{
	if (max <= 0) { return; }
	if (std::abs(src - target) < max) {
		src = target;
	}
	else if (src > target) {
		src -= max;
	}
	else {
		src += max;
	}
}

void MoveTowards(int& src, int target, int max)
{
	if (max <= 0) { return; }
	if (std::abs(src - target) < max) {
		src = target;
	}
	else if (src > target) {
		src -= max;
	}
	else {
		src += max;
	}
}

bool SameSign(int a, int b) {
	return !((a ^ b) < 0);
}