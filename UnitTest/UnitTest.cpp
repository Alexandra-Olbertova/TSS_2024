#include "pch.h"
#include "CppUnitTest.h"
#include "../MFC/HistogramCalc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
    TEST_CLASS(UnitTest)
    {
    public:
// 1. UnitTest - only blue pixels		
        TEST_METHOD(UnitTest_RedPixels)
        {
            const int width = 2;
            const int height = 1;
            const int stride = width * 4;
            BYTE pixelData[] = {
                255, 0, 0, 255, // blue pixel 
                255, 0, 0, 255  // blue pixel
            };

            int histogramR[256] = { 0 };
            int histogramG[256] = { 0 };
            int histogramB[256] = { 0 };

            CalculateHistogramData(pixelData, width, height, stride, histogramR, histogramG, histogramB);

            Assert::AreEqual(0, histogramR[255]); // 0 red pixels
            Assert::AreEqual(0, histogramG[255]); // 0 green pixels
            Assert::AreEqual(2, histogramB[255]); // 2 blue pixels
        }

        // 2. UnitTest - only white
        TEST_METHOD(UnitTest_AllWhite)
        {
            int width = 2;
            int height = 2;
            const int stride = width * 4;
            BYTE pixelData[] = {
                255, 255, 255, 255,  // White
                255, 255, 255, 255,  // White
                255, 255, 255, 255,  // White
                255, 255, 255, 255   // White
            };

            int histogramR[256] = { 0 };
            int histogramG[256] = { 0 };
            int histogramB[256] = { 0 };

            CalculateHistogramData(pixelData, width, height, stride, histogramR, histogramG, histogramB);

            Assert::AreEqual(4, histogramR[255]);
            Assert::AreEqual(4, histogramG[255]);
            Assert::AreEqual(4, histogramB[255]);
        }

        // 3. UnitTest 
        TEST_METHOD(UnitTest_DifferentColors)
        {
            int width = 2;
            int height = 2;
            const int stride = width * 4;
            BYTE pixelData[] = {
                128, 30, 40, 255,
                20, 128, 10, 255, 
                150, 30, 40, 255, 
                240, 90, 40, 255  
            };

            int histogramR[256] = { 0 };
            int histogramG[256] = { 0 };
            int histogramB[256] = { 0 };

            CalculateHistogramData(pixelData, width, height, stride, histogramR, histogramG, histogramB);

            Assert::AreEqual(1, histogramB[128]); 
            Assert::AreEqual(1, histogramB[20]);
            Assert::AreEqual(1, histogramB[150]);
            Assert::AreEqual(1, histogramB[240]);

            Assert::AreEqual(2, histogramG[30]); 
            Assert::AreEqual(1, histogramG[128]);
            Assert::AreEqual(1, histogramG[90]);

            Assert::AreEqual(3, histogramR[40]); 
            Assert::AreEqual(1, histogramR[10]);
        }

    };
}
