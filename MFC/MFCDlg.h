
// MFCDlg.h : header file
//
#include <vector>
#include <mutex>


#pragma once

// zadefinovana hodnota konstant - vsade bude mat definovanu danu hodnotu
enum
{
	WM_DRAW_IMAGE = WM_USER + 1,
	WM_DRAW_HISTOGRAM, // ak nepripocitame, tak automaticky prirata +1 (WM_DRAW_IMAGE + 1 , WM_USER + 2)
	WM_HISTOGRAM_CALCULATION_DONE,
	WM_MOSAIC_DONE
};

class CStaticImage : public CStatic
{
public:
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
};

class CStaticHist : public CStatic
{
public:
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
};

struct Histogram {
	int r[256] = { 0 };
	int g[256] = { 0 };
	int b[256] = { 0 };
};

struct Img
{
	CString fileName;
	CString filePath;

	Gdiplus::Image* imageBitmap;
	Histogram histogram;

	std::vector<Gdiplus::Image*> imageBitmapMosaic = std::vector<Gdiplus::Image*>(4, nullptr);

	// uvolnenie pamate
	~Img() {
		for (auto& mosaicImage : imageBitmapMosaic) {
			mosaicImage = nullptr;
			delete mosaicImage;
		}
	}

	bool histogramCalculated = false;
	bool histogramCalculationInProgress = false;

	bool mosaicProcessing = false;
	bool mosaicDelete = false;
};

// CMFCDlg dialog
class CMFCDlg : public CDialogEx
{
	// Construction
public:
	CMFCDlg(CWnd* pParent = nullptr);	// standard constructor

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CRect m_rectFileList;
	CRect m_rect;

	CRect m_rectStaticImage;
	CRect m_rectStaticHistogram;

	CListCtrl m_fileList;
	CStaticHist m_staticHistogram;
	CStaticImage m_staticImage;

	std::vector<Img> m_imageList;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;


	// histogram
	bool m_histogramR_checked;
	bool m_histogramG_checked;
	bool m_histogramB_checked;

	void CalculateHistogram(Img& image);
	void DrawHistogram(Gdiplus::Graphics* gr, const CRect& rect, int* histogram, Gdiplus::Color color);

	void HistogramCalculationThread();

	bool m_mosaic_checked_10;
	bool m_mosaic_checked_20;
	bool m_mosaic_checked_30;
	bool m_mosaic_checked_40;

	int GetBlockSizeIndex(int blockSize);
	void ApplyMosaicEffect(Gdiplus::Bitmap* bitmap, int blockSize);
	void ApplyMosaicEffectBasedOnSelection();
	void applyMosaicInThread(int selectedItemIndex, int blockSize);
	void ResetMosaicFlags();



	afx_msg void OnOpen();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	//CMFCDlg
	afx_msg LRESULT OnDrawImage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDrawHist(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHistogramCalculationDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMosaicDone(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHistogramR32788();
	afx_msg void OnHistogramG32789();
	afx_msg void OnHistogramB32790();
	afx_msg void OnObrazokMosaic();
	afx_msg void OnObrazokMosaic20();
	afx_msg void OnObrazokMosaic30();
	afx_msg void OnObrazokMosaic40();
	afx_msg void OnObrazokResetmosaic();
};