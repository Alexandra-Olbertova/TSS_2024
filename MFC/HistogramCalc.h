#pragma once
#include <gdiplus.h>

struct Histogram {
	int r[256] = { 0 };
	int g[256] = { 0 };
	int b[256] = { 0 };
};

struct Img;

void CalculateHistogram(Img& image);