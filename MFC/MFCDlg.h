
// MFCDlg.h : header file
//
#include <vector>
#pragma once

// zadefinovana hodnota konstant - vsade bude mat definovanu danu hodnotu
enum
{
	WM_DRAW_IMAGE = WM_USER + 1,
	WM_DRAW_HISTOGRAM // ak nepripocitame, tak automaticky prirata +1 (WM_DRAW_IMAGE + 1 , WM_USER + 2)
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

// trieda pre ukladanie suboru
class File 
{
public:
	CString path;
	CString fileName;

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

	std::vector<File> m_fileListVector;

	afx_msg void OnOpen();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	//CMFCDlg
	afx_msg LRESULT OnDrawImage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDrawHist(WPARAM wParam, LPARAM lParam);
};



