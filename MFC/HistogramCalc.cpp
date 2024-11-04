#include "pch.h"
#include "HistogramCalc.h"
#include "MFCDlg.h"
#include <gdiplus.h>

using namespace Gdiplus;

void CalculateHistogram(Img& image)
{
	if (image.imageBitmap == nullptr) return;

	Bitmap* bmp = static_cast<Bitmap*>(image.imageBitmap);
	UINT width = bmp->GetWidth();
	UINT height = bmp->GetHeight();

	// LockBits
	Rect rect(0, 0, width, height);
	BitmapData bitmapData;

	if (bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData) == Gdiplus::Ok)
	{
		BYTE* pixels = static_cast<BYTE*>(bitmapData.Scan0);

		for (UINT y = 0; y < height; ++y)
		{
			BYTE* row = pixels + y * bitmapData.Stride;
			for (UINT x = 0; x < width; ++x)
			{
				BYTE blue = row[x * 4];
				BYTE green = row[x * 4 + 1];
				BYTE red = row[x * 4 + 2];

				image.histogram.b[blue]++;
				image.histogram.g[green]++;
				image.histogram.r[red]++;
			}
		}

		bmp->UnlockBits(&bitmapData);
	}
}
