// MFCDlg.cpp : implementation file
//

// kill thread - thread nieco bude vracat 
// mutex premenna 

#include "pch.h"
#include "framework.h"
#include "MFC.h"
#include "MFCDlg.h"
#include "HistogramCalc.h"
#include "afxdialogex.h"
#include <gdiplus.h>
#include <thread>

using namespace Gdiplus;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void CStaticImage::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	GetParent()->SendMessage(WM_DRAW_IMAGE, (WPARAM)lpDrawItemStruct);
}

void CStaticHist::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	GetParent()->SendMessage(WM_DRAW_HISTOGRAM, (WPARAM)lpDrawItemStruct);
}


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	CRect m_rect;
	//CRect m_rectFileList;
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCDlg dialog

// afx_msg LRESULT OnDrawImage(WPARAM wParam, LPARAM lParam);
// afx_msg LRESULT OnDrawHist(WPARAM wParam, LPARAM lParam);

LRESULT CMFCDlg::OnDrawImage(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT st = (LPDRAWITEMSTRUCT)wParam;

	auto gr = Gdiplus::Graphics::FromHDC(st->hDC);

	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedItemIndex == -1) return S_OK;

	Img& selectedFile = m_imageList[selectedItemIndex];

	if (selectedFile.imageBitmap)
	{
		CRect rect;
		m_staticImage.GetClientRect(&rect);

		// velkost obrazka
		REAL imgWidth = selectedFile.imageBitmap->GetWidth();
		REAL imgHeight = selectedFile.imageBitmap->GetHeight();

		REAL drawWidth, drawHeight, nDiffX = 0, nDiffY = 0;

		//centrovanie obrazka
		REAL imageRatio = imgWidth / imgHeight;
		REAL rectRatio = rect.Width() / rect.Height();
		if (imageRatio > rectRatio)
		{
			drawWidth = rect.Width();
			drawHeight = drawWidth / imageRatio;
			nDiffY = (rect.Height() - drawHeight) / 2;
		}
		else
		{
			drawHeight = rect.Height();
			drawWidth = drawHeight * imageRatio;
			nDiffX = (rect.Width() - drawWidth) / 2;
		}

		drawWidth = min(drawWidth, rect.Width());
		drawHeight = min(drawHeight, rect.Height());

		if(m_mosaic_checked_10 || m_mosaic_checked_20 || m_mosaic_checked_30 || m_mosaic_checked_40)
			gr->DrawImage(selectedFile.imageBitmapMosaic, rect.left + nDiffX, rect.top + nDiffY, drawWidth, drawHeight);
		else
			gr->DrawImage(selectedFile.imageBitmap, rect.left + nDiffX, rect.top + nDiffY, drawWidth, drawHeight);

	}

	return S_OK;
}

LRESULT CMFCDlg::OnDrawHist(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT st = (LPDRAWITEMSTRUCT)wParam;

	auto gr = Gdiplus::Graphics::FromHDC(st->hDC);
	CRect rect;
	m_staticHistogram.GetClientRect(&rect);

	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);


	// podla checked sa vykresli dany histogram

	if (m_histogramR_checked) {
		DrawHistogram(gr, rect, m_imageList[selectedItemIndex].histogram.r, Gdiplus::Color(255, 0, 0));
	}
	if (m_histogramG_checked) {
		DrawHistogram(gr, rect, m_imageList[selectedItemIndex].histogram.g, Gdiplus::Color(0, 255, 0));
	}
	if (m_histogramB_checked) {
		DrawHistogram(gr, rect, m_imageList[selectedItemIndex].histogram.b, Gdiplus::Color(0, 0, 255));
	}

	return S_OK;
}

LRESULT CMFCDlg::OnHistogramCalculationDone(WPARAM wParam, LPARAM lParam)
{
	/*int doneIndex = static_cast<int>(wParam);

	//m_imageList[doneIndex].histogramCalculationInProgress = false;
	//m_imageList[doneIndex].histogramCalculated = true;

	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);

	if (selectedItemIndex == doneIndex) {
		m_staticHistogram.Invalidate(FALSE);
		InvalidateRect(nullptr, TRUE);
	}
	*/
	m_staticHistogram.Invalidate(FALSE);
	m_staticImage.Invalidate(FALSE);
	return 0;
}

LRESULT CMFCDlg::OnMosaicDone(WPARAM wParam, LPARAM lParam)
{
	m_staticImage.Invalidate(FALSE);
	m_staticHistogram.Invalidate(FALSE);

	return 0;
}


CMFCDlg::CMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_DIALOG, pParent),

	// prvotne nastavenie -> vsetky zlozky budu unchecked
	m_histogramR_checked(false),
	m_histogramG_checked(false),
	m_histogramB_checked(false),
	
	m_mosaic_checked_10(false),
	m_mosaic_checked_20(false),
	m_mosaic_checked_30(false),
	m_mosaic_checked_40(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, m_fileList);
	DDX_Control(pDX, IDC_STATIC_HISTOGRAM, m_staticHistogram);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_staticImage);
}

BEGIN_MESSAGE_MAP(CMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_OPEN, &CMFCDlg::OnOpen)
	ON_COMMAND(ID_CLOSE, &CMFCDlg::OnClose)
	ON_MESSAGE(WM_DRAW_IMAGE, OnDrawImage)
	ON_MESSAGE(WM_DRAW_HISTOGRAM, OnDrawHist)
	ON_WM_SIZE()
	ON_WM_DRAWITEM()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, &CMFCDlg::OnLvnItemchangedFileList)
	ON_COMMAND(ID_HISTOGRAM_R32788, &CMFCDlg::OnHistogramR32788)
	ON_COMMAND(ID_HISTOGRAM_G32789, &CMFCDlg::OnHistogramG32789)
	ON_COMMAND(ID_HISTOGRAM_B32790, &CMFCDlg::OnHistogramB32790)
	ON_MESSAGE(WM_HISTOGRAM_CALCULATION_DONE, &CMFCDlg::OnHistogramCalculationDone)
	ON_MESSAGE(WM_MOSAIC_DONE, &CMFCDlg::OnMosaicDone)
	ON_COMMAND(ID_OBRAZOK_MOSAIC_10, &CMFCDlg::OnObrazokMosaic)
	ON_COMMAND(ID_OBRAZOK_MOSAIC_20, &CMFCDlg::OnObrazokMosaic20)
	ON_COMMAND(ID_OBRAZOK_MOSAIC_30, &CMFCDlg::OnObrazokMosaic30)
	ON_COMMAND(ID_OBRAZOK_MOSAIC_40, &CMFCDlg::OnObrazokMosaic40)
	ON_COMMAND(ID_OBRAZOK_RESETMOSAIC, &CMFCDlg::OnObrazokResetmosaic)
END_MESSAGE_MAP()


// CMFCDlg message handlers

BOOL CMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// histogram - checked / unchecked
	CMenu* pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_HISTOGRAM_R32788, m_histogramR_checked ? MF_CHECKED : MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_HISTOGRAM_G32789, m_histogramG_checked ? MF_CHECKED : MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_HISTOGRAM_B32790, m_histogramB_checked ? MF_CHECKED : MF_UNCHECKED);

	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_10, m_mosaic_checked_10 ? MF_CHECKED : MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_20, m_mosaic_checked_20 ? MF_CHECKED : MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_30, m_mosaic_checked_30 ? MF_CHECKED : MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_40, m_mosaic_checked_40 ? MF_CHECKED : MF_UNCHECKED);

	// chcem vediet velkost hlavnej aplikacie
	GetClientRect(&m_rect);
	m_fileList.GetWindowRect(&m_rectFileList);
	m_staticImage.GetWindowRect(&m_rectStaticImage);
	m_staticHistogram.GetWindowRect(&m_rectStaticHistogram);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCDlg::OnOpen()
{
	CString filter = _T("Files (*.png;*.bmp;*.jpg)|*.png;*.bmp;*.jpg||");

	CFileDialog fileDlg(TRUE, _T("Select File"), NULL, OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, filter);

	if (fileDlg.DoModal() == IDOK)
	{
		POSITION pos = fileDlg.GetStartPosition();
		while (pos != NULL)
		{
			CString selectedPath = fileDlg.GetNextPathName(pos);

			// ulozi cestu a nazov 
			Img image;
			image.filePath = selectedPath.Left(selectedPath.ReverseFind('\\'));
			image.fileName = selectedPath.Mid(selectedPath.ReverseFind('\\') + 1);

			CString fullPath = image.filePath + "\\" + image.fileName;
			image.imageBitmap = Gdiplus::Image::FromFile(fullPath);
			image.imageBitmapMosaic = Gdiplus::Image::FromFile(fullPath);

			// zisti ci sa uz vo vectore nachadza 
			bool fileExists = false;
			for (const auto& i : m_imageList)
			{
				if (i.filePath == image.filePath && i.fileName == image.fileName)
				{
					fileExists = true;
					break;
				}
			}

			if (!fileExists)
			{
				m_imageList.push_back(image);
				int itemIndex = m_fileList.GetItemCount();
				m_fileList.InsertItem(itemIndex, image.fileName); // vypise vo fileListe
			}
			else
			{
				AfxMessageBox(_T("File is already open: ") + image.fileName);
			}
		}
	}
	else
	{
		AfxMessageBox(_T("No file selected"));
	}
}

void CMFCDlg::OnClose()
{
	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);

	if (selectedItemIndex == -1)
	{
		AfxMessageBox(_T("No file selected"));
		return;
	}

	CString selectedFileName = m_fileList.GetItemText(selectedItemIndex, 0);

	// spyta sa, ci si prajete odstranit subor po potvrdenie - yes/no message button
	CString message;
	message.Format(_T("Do you want to remove the file: %s?"), selectedFileName);

	if (AfxMessageBox(message, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		// subor sa zo zoznamu odstrani
		auto i = std::remove_if(m_imageList.begin(), m_imageList.end(),
			[&](const Img& file)
			{
				return file.fileName == selectedFileName;
			});

		m_imageList.erase(i, m_imageList.end());

		if (::IsWindow(m_fileList.m_hWnd)) {
			m_fileList.DeleteItem(selectedItemIndex);

			// po vymazani sa oznaci prvy 
			int remainingItems = m_fileList.GetItemCount();
			if (remainingItems > 0)
			{
				m_fileList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
			}
			else
			{
				m_staticImage.Invalidate();
				m_staticHistogram.Invalidate();

				m_histogramG_checked = false;
				m_histogramB_checked = false;
				m_histogramR_checked = false;
				
				m_mosaic_checked_10 = false;
				m_mosaic_checked_20 = false;
				m_mosaic_checked_30 = false;
				m_mosaic_checked_40 = false;

				CMenu* pMenu = GetMenu();
				pMenu->CheckMenuItem(ID_HISTOGRAM_R32788, MF_UNCHECKED);
				pMenu->CheckMenuItem(ID_HISTOGRAM_G32789, MF_UNCHECKED);
				pMenu->CheckMenuItem(ID_HISTOGRAM_B32790, MF_UNCHECKED);

				pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_10, MF_UNCHECKED);
				pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_20, MF_UNCHECKED);
				pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_30, MF_UNCHECKED);
				pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_40, MF_UNCHECKED);
			}

			Invalidate(FALSE);
		}
	}
}

void CMFCDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	int nDiffY = cy - m_rect.Height();
	int nDiffX = cx - m_rect.Width();

	if (::IsWindow(m_fileList) && ::IsWindow(m_staticImage) && ::IsWindow(m_staticHistogram)) {

		m_fileList.SetWindowPos(nullptr, 0, 0, m_rectFileList.Width(), m_rectFileList.Height() + nDiffY, SWP_NOMOVE);

		m_staticImage.SetWindowPos(nullptr, 0, 0, m_rectStaticImage.Width() + nDiffX, m_rectStaticImage.Height() + nDiffY, SWP_NOMOVE);

		int histDiff = m_rectFileList.Height() - m_rectStaticHistogram.Height();
		int histRight = cy - m_rectStaticHistogram.Height() - histDiff;

		m_staticHistogram.SetWindowPos(nullptr, m_rectStaticHistogram.left - histDiff, histRight, m_rectStaticHistogram.Width(), m_rectStaticHistogram.Height(), SWP_NOZORDER);

	}

	Invalidate(TRUE);
}

void CMFCDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMFCDlg::OnLvnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (m_histogramR_checked || m_histogramG_checked || m_histogramB_checked) {
		HistogramCalculationThread();

	}

	if (m_mosaic_checked_10 || m_mosaic_checked_20 || m_mosaic_checked_30 || m_mosaic_checked_40) {
		ApplyMosaicEffectBasedOnSelection();
	}

	m_staticImage.Invalidate(FALSE);
	m_staticHistogram.Invalidate(FALSE);

	InvalidateRect(nullptr, TRUE);

	*pResult = 0;
}

void CMFCDlg::HistogramCalculationThread()
{
	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);

	if (selectedItemIndex != -1) {
		Img& selectedImage = m_imageList[selectedItemIndex];

		// ak je spusteny vypocet, dalsi sa nespusti
		if (selectedImage.histogramCalculated || selectedImage.histogramCalculationInProgress) { return; }

		selectedImage.histogramCalculationInProgress = true;

		std::thread thread_hist([this, selectedItemIndex]() {
			CalculateHistogram(m_imageList[selectedItemIndex]);
			PostMessage(WM_HISTOGRAM_CALCULATION_DONE, selectedItemIndex, 0);
			});

		thread_hist.detach();

		selectedImage.histogramCalculationInProgress = false;
		selectedImage.histogramCalculated = true;

	}
}

void CMFCDlg::DrawHistogram(Gdiplus::Graphics* gr, const CRect& rect, int* histogram, Gdiplus::Color color)
{
	int maxCount = *std::max_element(histogram, histogram + 256);
	float barWidth = static_cast<float>(rect.Width()) / 256;

	// nakreslenie histogramu
	for (int i = 0; i < 256; ++i) {
		float barHeight = (static_cast<float>(histogram[i]) / maxCount) * rect.Height();
		gr->FillRectangle(&SolidBrush(color), i * barWidth, rect.Height() - barHeight, barWidth, barHeight);
	}
}

void CMFCDlg::OnHistogramR32788()
{
	m_histogramR_checked = !m_histogramR_checked;
	CMenu* pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_HISTOGRAM_R32788, m_histogramR_checked ? MF_CHECKED : MF_UNCHECKED);

	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedItemIndex >= 0) {
		Img& selectedImage = m_imageList[selectedItemIndex];
		if (!selectedImage.histogramCalculated && !selectedImage.histogramCalculationInProgress) {
			HistogramCalculationThread();  
		}
		Invalidate(); 
	}
}

void CMFCDlg::OnHistogramG32789()
{
	m_histogramG_checked = !m_histogramG_checked;
	CMenu* pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_HISTOGRAM_G32789, m_histogramG_checked ? MF_CHECKED : MF_UNCHECKED);

	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedItemIndex >= 0) {
		Img& selectedImage = m_imageList[selectedItemIndex];
		if (!selectedImage.histogramCalculated && !selectedImage.histogramCalculationInProgress) {
			HistogramCalculationThread();
		}
		Invalidate();
	}
}

void CMFCDlg::OnHistogramB32790()
{
	m_histogramB_checked = !m_histogramB_checked;
	CMenu* pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_HISTOGRAM_B32790, m_histogramB_checked ? MF_CHECKED : MF_UNCHECKED);

	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedItemIndex >= 0) {
		Img& selectedImage = m_imageList[selectedItemIndex];
		if (!selectedImage.histogramCalculated && !selectedImage.histogramCalculationInProgress) {
			HistogramCalculationThread();
		}
		Invalidate();
	}
}

void CMFCDlg::CalculateHistogram(Img& image)
{
	//std::this_thread::sleep_for(std::chrono::seconds(5));

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

		CalculateHistogramData(pixels, width, height, bitmapData.Stride, image.histogram.r, image.histogram.g, image.histogram.b);

		bmp->UnlockBits(&bitmapData);
	}

	image.histogramCalculated = true;
	image.histogramCalculationInProgress = false;
}

void CMFCDlg::ApplyMosaicEffect(Bitmap* bitmap, int blockSize)
{
	// obrazok s N pixelmi 
	// kazdy pixel je reprezentovany RGB
	// avgRGB vypocitam ako priemer jednotlivych hodnot - (r1 + ... + rN) / N, ...
	
	//std::this_thread::sleep_for(std::chrono::seconds(6));

	UINT width = bitmap->GetWidth();
	UINT height = bitmap->GetHeight();

	Rect rect(0, 0, width, height);
	BitmapData bitmapData;

	if (bitmap->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData) == Gdiplus::Ok)
	{
		BYTE* pixels = static_cast<BYTE*>(bitmapData.Scan0);
		int stride = bitmapData.Stride;

		for (UINT y = 0; y < height; y += blockSize){// posuvam sa o velkost bloku
			for (UINT x = 0; x < width; x += blockSize){
				int rSum = 0, gSum = 0, bSum = 0, pixelCount = 0;

				// priemer
				for (UINT yy = y; yy < y + blockSize && yy < height; yy++){
					for (UINT xx = x; xx < x + blockSize && xx < width; xx++){
						BYTE* pixel = pixels + (yy * stride) + (xx * 4);

						bSum += pixel[0];
						gSum += pixel[1];
						rSum += pixel[2];
						pixelCount++;
					}
				}
				BYTE rAvg = static_cast<BYTE>(rSum / pixelCount);
				BYTE gAvg = static_cast<BYTE>(gSum / pixelCount);
				BYTE bAvg = static_cast<BYTE>(bSum / pixelCount);

				// aplikovanie
				for (UINT yy = y; yy < y + blockSize && yy < height; yy++){
					for (UINT xx = x; xx < x + blockSize && xx < width; xx++){
						BYTE* pixel = pixels + (yy * stride) + (xx * 4);

						pixel[0] = bAvg; // blue
						pixel[1] = gAvg; // green
						pixel[2] = rAvg; // red
					}
				}
			}
		}

		bitmap->UnlockBits(&bitmapData);
	}
}

/*
void CMFCDlg::ApplyMosaicEffectBasedOnSelection() {

	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedItemIndex != -1) {
		Img& selectedImage = m_imageList[selectedItemIndex];

		delete selectedImage.imageBitmapMosaic; 
		selectedImage.imageBitmapMosaic = selectedImage.imageBitmap->Clone();

		if (m_mosaic_checked_10) {

			std::thread thread_mosaic_10([this, selectedImage]() {
				ApplyMosaicEffect(static_cast<Bitmap*>(selectedImage.imageBitmapMosaic), 10);
				PostMessage(WM_MOSAIC_DONE, selectedImage, 0);
				});

			thread_mosaic_10.detach();
		}
		else if (m_mosaic_checked_20) {
			ApplyMosaicEffect(static_cast<Bitmap*>(selectedImage.imageBitmapMosaic), 20);
		}
		else if (m_mosaic_checked_30) {
			ApplyMosaicEffect(static_cast<Bitmap*>(selectedImage.imageBitmapMosaic), 30);
		}
		else if (m_mosaic_checked_40) {
			ApplyMosaicEffect(static_cast<Bitmap*>(selectedImage.imageBitmapMosaic), 40);
		}
		Invalidate();
	}
}

*/

void CMFCDlg::applyMosaicInThread(int selectedItemIndex, int blockSize) {

	std::thread mosaicThread([this, selectedItemIndex, blockSize]() {
		Img& selectedImageThread = m_imageList[selectedItemIndex];

		ApplyMosaicEffect(static_cast<Bitmap*>(selectedImageThread.imageBitmapMosaic), blockSize);

		PostMessage(WM_MOSAIC_DONE, selectedItemIndex, 0);
		});

	mosaicThread.detach();
}

void CMFCDlg::ApplyMosaicEffectBasedOnSelection() {

	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);

	if (selectedItemIndex != -1) {
		Img& selectedImage = m_imageList[selectedItemIndex];

		delete selectedImage.imageBitmapMosaic;
		selectedImage.imageBitmapMosaic = selectedImage.imageBitmap->Clone();

		if (m_mosaic_checked_10) {
			applyMosaicInThread(selectedItemIndex, 10);
		}
		else if (m_mosaic_checked_20) {
			applyMosaicInThread(selectedItemIndex, 20);
		}
		else if (m_mosaic_checked_30) {
			applyMosaicInThread(selectedItemIndex, 30);
		}
		else if (m_mosaic_checked_40) {
			applyMosaicInThread(selectedItemIndex, 40);
		}
		Invalidate();
	}
}


void CMFCDlg::OnObrazokMosaic() {

	ResetMosaicFlags();

	m_mosaic_checked_10 = true;

	CMenu* pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_10, MF_CHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_20, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_30, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_40, MF_UNCHECKED);

	ApplyMosaicEffectBasedOnSelection();
}

void CMFCDlg::OnObrazokMosaic20() {

	ResetMosaicFlags();

	m_mosaic_checked_20 = true;

	CMenu* pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_10, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_20, MF_CHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_30, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_40, MF_UNCHECKED);

	ApplyMosaicEffectBasedOnSelection();
}

void CMFCDlg::OnObrazokMosaic30()
{

	ResetMosaicFlags();

	m_mosaic_checked_30 = true;

	CMenu* pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_10, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_20, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_30, MF_CHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_40, MF_UNCHECKED);

	ApplyMosaicEffectBasedOnSelection();
}

void CMFCDlg::OnObrazokMosaic40()
{

	ResetMosaicFlags();

	m_mosaic_checked_40 = true;

	CMenu* pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_10, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_20, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_30, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_40, MF_CHECKED);

	ApplyMosaicEffectBasedOnSelection();
}

void CMFCDlg::OnObrazokResetmosaic(){
	
	ResetMosaicFlags();

	CMenu* pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_10, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_20, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_30, MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_OBRAZOK_MOSAIC_40, MF_UNCHECKED);

	int selectedItemIndex = m_fileList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedItemIndex != -1) {
		Img& selectedImage = m_imageList[selectedItemIndex];

		delete selectedImage.imageBitmapMosaic; 
		selectedImage.imageBitmapMosaic = nullptr;

		m_staticImage.Invalidate(FALSE);
	}
}

void CMFCDlg::ResetMosaicFlags() {
	m_mosaic_checked_10 = false;
	m_mosaic_checked_20 = false;
	m_mosaic_checked_30 = false;
	m_mosaic_checked_40 = false;
}

