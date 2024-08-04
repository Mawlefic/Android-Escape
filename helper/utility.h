#pragma once
#include "raylib.h"

// both min and max exclusive
bool InRange(const int value, const int min, const int max);
bool ColorEqual(const Color& c1, const Color& c2);
void MoveTowards(float& src, float target, float max);
void MoveTowards(int& src, int target, int max);
bool SameSign(int a, int b);