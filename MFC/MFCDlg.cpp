// inicializacia gdiplus init

// MFCDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFC.h"
#include "MFCDlg.h"
#include "afxdialogex.h"


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

	// pridanie novej kniznice Gdi
	auto gr = Gdiplus::Graphics::FromHDC(st->hDC);

	// image
	//gr->DrawImage();

	// histogram
	//gr->DrawCurve();

	return S_OK;
}

LRESULT CMFCDlg::OnDrawHist(WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}


CMFCDlg::CMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_DIALOG, pParent)
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
END_MESSAGE_MAP()


// CMFCDlg message handlers

BOOL CMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

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
			File file;
			file.path = selectedPath.Left(selectedPath.ReverseFind('\\'));
			file.fileName = selectedPath.Mid(selectedPath.ReverseFind('\\') + 1);

			// zisti ci sa uz vo vectore nachadza 
			bool fileExists = false;
			for (const auto& i : m_fileListVector)
			{
				if (i.path == file.path && i.fileName == file.fileName)
				{
					fileExists = true;
					break;
				}
			}

			if (!fileExists)
			{

				m_fileListVector.push_back(file);
				int itemIndex = m_fileList.GetItemCount();
				m_fileList.InsertItem(itemIndex, file.fileName); // vypise vo fileListe

				//AfxMessageBox(_T("Selected File: ") + file.fileName);
			}
			else
			{
				AfxMessageBox(_T("File is already open: ") + file.fileName);
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
	}
	else
	{
		CString selectedFileName = m_fileList.GetItemText(selectedItemIndex, 0);

		// spyta sa, ci si prajete odstranit subor po potvrdenie - yes/no message button
		CString message;
		message.Format(_T("Do you want to remove the file: %s?"), selectedFileName);

		if (AfxMessageBox(message, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			// subor sa zo zoznamu odstrani
			auto i = std::remove_if(m_fileListVector.begin(), m_fileListVector.end(),
				[&](const File& file)
				{
					return file.fileName == selectedFileName;
				});
			m_fileListVector.erase(i, m_fileListVector.end());
			m_fileList.DeleteItem(selectedItemIndex);
		}
		AfxMessageBox(_T("File removed successfully."));

		// po vymazani sa oznaci prvy 
		int remainingItems = m_fileList.GetItemCount();
		if (remainingItems > 0)
		{
			m_fileList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
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
	
	//m_staticImage.Invalidate(FALSE); // zavola sa OnDraw

	*pResult = 0;
}
