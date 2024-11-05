#ifndef HISTOGRAMCALC_H
#define HISTOGRAMCALC_H

#include <Windows.h>


void CalculateHistogramData(const BYTE* pixelData, int width, int height, int stride, int* histogramR, int* histogramG, int* histogramB) {

    for (int y = 0; y < height; ++y) {
        const BYTE* row = pixelData + y * stride;
        for (int x = 0; x < width; ++x) {
            BYTE blue = row[x * 4];
            BYTE green = row[x * 4 + 1];
            BYTE red = row[x * 4 + 2];

            histogramB[blue]++;
            histogramG[green]++;
            histogramR[red]++;
        }
    }
}

#endif 
