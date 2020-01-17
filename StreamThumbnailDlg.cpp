
// StreamThumbnailDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "StreamThumbnail.h"
#include "StreamThumbnailDlg.h"
#include "afxdialogex.h"
#include "GetStreamThumbnail.h"
#include "PicConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CStreamThumbnailDlg 对话框



CStreamThumbnailDlg::CStreamThumbnailDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStreamThumbnailDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nPngCount = 0;
}

void CStreamThumbnailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStreamThumbnailDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHOOSE_SAVEPATH, &CStreamThumbnailDlg::OnBnClickedChooseSavepath)
	ON_BN_CLICKED(IDC_OPEN_STREAM, &CStreamThumbnailDlg::OnBnClickedOpenStream)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHOOSE_JPG, &CStreamThumbnailDlg::OnBnClickedChooseJpg)
	ON_BN_CLICKED(IDC_CHOOSE_SAVEBMP, &CStreamThumbnailDlg::OnBnClickedChooseSavebmp)
	ON_BN_CLICKED(IDC_BTN_CONVERT, &CStreamThumbnailDlg::OnBnClickedBtnConvert)
	ON_BN_CLICKED(IDC_CHOOSE_SAVE, &CStreamThumbnailDlg::OnBnClickedChooseSave)
	ON_BN_CLICKED(IDC_BTN_CONVERT2, &CStreamThumbnailDlg::OnBnClickedBtnConvert2)
	ON_BN_CLICKED(IDC_BUTTON_PICTOYUV, &CStreamThumbnailDlg::OnBnClickedButtonPictoyuv)
	ON_BN_CLICKED(IDC_CHOOSE_JPG2, &CStreamThumbnailDlg::OnBnClickedChooseJpg2)
END_MESSAGE_MAP()


// CStreamThumbnailDlg 消息处理程序

BOOL CStreamThumbnailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	GetStreamThumbnail::Initialize();

	InitParam();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CStreamThumbnailDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CStreamThumbnailDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CStreamThumbnailDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CStreamThumbnailDlg::InitParam()
{
	SetDlgItemText(IDC_EDIT_STREAMADDR, L"rtmp://live.hkstv.hk.lxdns.com/live/hks");
	SetDlgItemText(IDC_EDIT_SAVEPATH, L"D:\\StreamThumbnail");
	SetDlgItemText(IDC_EDIT_DSTPATH, L"D:\\StreamThumbnail");
	SetDlgItemInt(IDC_EDIT_TIMEOUT, 3);
	SetDlgItemInt(IDC_EDIT_OUTHEIGHT, 0);
	SetDlgItemInt(IDC_EDIT_OUTWIDTH, 0);

	//SetDlgItemText(IDC_EDIT_JPGFILEPATH, L"IMG_3475.jpg");
	SetDlgItemText(IDC_EDIT_JPGFILEPATH, L"test.yuv");

	SetDlgItemInt(IDC_EDIT_OUTHEIGHT2, 0);
	SetDlgItemInt(IDC_EDIT_OUTWIDTH2, 0);

	MakeSureDirExist(L"D:\\StreamThumbnail");
	m_nPngCount = 0;

	SetDlgItemText(IDC_EDIT_JPGFILEPATH2, L"tt.jpg");
	SetDlgItemText(IDC_EDIT_DSTPATH2, L"result.yuv");
	/*SetDlgItemInt(IDC_EDIT_STEP1_W, 1920);
	SetDlgItemInt(IDC_EDIT_STEP1_H, 1080);*/

	SetDlgItemText(IDC_EDIT_STEP2_X, L"0.25");
	SetDlgItemText(IDC_EDIT_STEP2_Y, L"0.25");
	SetDlgItemText(IDC_EDIT_STEP2_W, L"0.5");
	SetDlgItemText(IDC_EDIT_STEP2_H, L"0.5");

	SetDlgItemInt(IDC_EDIT_STEP3_W, 1920);
	SetDlgItemInt(IDC_EDIT_STEP3_H, 1080);
}

void CStreamThumbnailDlg::MakeSureDirExist(CString strSavePath)
{
	//判断是否存在，否则创建
	if (CreateDirectory(strSavePath, NULL))
	{
		TRACE(L"Create Folder : %s.\n", strSavePath);
	}
}

void CStreamThumbnailDlg::OnBnClickedChooseSavepath()
{
	TCHAR           szFolderPath[MAX_PATH] = { 0 };
	CString         strFolderPath = TEXT("");

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择流缩略图存储路径");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	SetDlgItemText(IDC_EDIT_SAVEPATH, strFolderPath);
	
	MakeSureDirExist(strFolderPath);
}


void CStreamThumbnailDlg::OnBnClickedOpenStream()
{
	CString strAddr, strPicPath;
	int nTimeOut, nOutWidth, nOutHeight;
	UpdateData();
	GetDlgItemText(IDC_EDIT_STREAMADDR, strAddr);
	GetDlgItemText(IDC_EDIT_SAVEPATH, strPicPath);
	nTimeOut = GetDlgItemInt(IDC_EDIT_TIMEOUT);
	nOutWidth = GetDlgItemInt(IDC_EDIT_OUTWIDTH);
	nOutHeight= GetDlgItemInt(IDC_EDIT_OUTHEIGHT);

	MakeSureDirExist(strPicPath);
	CString strPicName;
	strPicName.Format(L"\\Test%d.png", m_nPngCount++);
	strPicPath += strPicName;
	USES_CONVERSION;
	char* pAddr = T2A(strAddr);
	char* pPicPath = T2A(strPicPath);
	DWORD dw = GetTickCount();
	int nRet = GetStreamThumbnail::GetInstance()->GetPicFromStream(pAddr, pPicPath, nOutWidth, nOutHeight, nTimeOut);
	dw = GetTickCount() - dw;
	TRACE(L"nRet = %d, cost %d.\n", nRet, dw);
	if (nRet < 0)
	{
		CString str;
		str.Format(L"Get Image Failed, nRet = %d.", nRet);
		AfxMessageBox(str);
	}
	else
		TRACE(L"GET PIC SUCCESS.\n");
}


void CStreamThumbnailDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	GetStreamThumbnail::Uninitialize();
	PicConvert::Uninitialize();

	CDialogEx::OnClose();
}


void CStreamThumbnailDlg::OnBnClickedChooseJpg()
{
	CString filter;
	filter = L"JPG文件(*.jpg)|*.jpg|所有文件(*.*)|*.*||";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
	if (dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.GetPathName();
		SetDlgItemText(IDC_EDIT_JPGFILEPATH, strPath);
	}
}


void CStreamThumbnailDlg::OnBnClickedChooseSavebmp()
{
	TCHAR           szFolderPath[MAX_PATH] = { 0 };
	CString         strFolderPath = TEXT("");

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择BMP文件存储路径");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	SetDlgItemText(IDC_EDIT_BMPPATH, strFolderPath);

	MakeSureDirExist(strFolderPath);
}


void CStreamThumbnailDlg::OnBnClickedBtnConvert()
{
	CString strJPGPath, strBMPPath;
	UpdateData();
	GetDlgItemText(IDC_EDIT_JPGFILEPATH, strJPGPath);
	GetDlgItemText(IDC_EDIT_BMPPATH, strBMPPath);
	
	int nOutWidth = GetDlgItemInt(IDC_EDIT_OUTWIDTH2);
	int nOutHeight = GetDlgItemInt(IDC_EDIT_OUTHEIGHT2);

	CString strFileName;
	int nIndex = strJPGPath.ReverseFind('\\');
	int nIndexEnd = strJPGPath.ReverseFind('.');
	int nlen = strJPGPath.GetLength();
	strFileName.Format(L"%s", strJPGPath.Mid(nIndex + 1, nIndexEnd - nIndex - 1));
	
	if (strBMPPath.IsEmpty())
		strBMPPath = strJPGPath.Left(nIndex);
	else
		MakeSureDirExist(strBMPPath);

	CString strPicPath;
	if (!strBMPPath.IsEmpty())
		strPicPath.Format(L"%s\\%s.bmp", strBMPPath, strFileName);
		//strPicPath.Format(L"%s\\%s.png", strBMPPath, strFileName);
		//strPicPath.Format(L"%s.jpg", strFileName);
	else
		strPicPath.Format(L"%s.bmp", strFileName);
	USES_CONVERSION;
	char* pJPGPPath = T2A(strJPGPath);
	char* pBMPPath = T2A(strPicPath);
	DWORD dw = GetTickCount();
	int nRet= PicConvert::GetInstance()->PicToBmp24(pJPGPPath, pBMPPath, nOutWidth, nOutHeight);
	dw = GetTickCount() - dw;
	TRACE(L"nRet = %d, cost %d.\n", nRet, dw);
}


void CStreamThumbnailDlg::OnBnClickedChooseSave()
{
	TCHAR           szFolderPath[MAX_PATH] = { 0 };
	CString         strFolderPath = TEXT("");

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择目标文件存储路径");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	SetDlgItemText(IDC_EDIT_DSTPATH, strFolderPath);

	MakeSureDirExist(strFolderPath);
}


void CStreamThumbnailDlg::OnBnClickedBtnConvert2()
{
	CString strJPGPath, strDstPath;
	UpdateData();
	GetDlgItemText(IDC_EDIT_JPGFILEPATH, strJPGPath);
	GetDlgItemText(IDC_EDIT_DSTPATH, strDstPath);

	if (strJPGPath.IsEmpty() || strDstPath.IsEmpty())
	{
		AfxMessageBox(_T("检测源路径和目标路径是否正确"));
		return;
	}

	int nOutWidth = GetDlgItemInt(IDC_EDIT_OUTWIDTH2);
	int nOutHeight = GetDlgItemInt(IDC_EDIT_OUTHEIGHT2);

	CString strFileName;
	int nIndex = strJPGPath.ReverseFind('\\');
	int nIndexEnd = strJPGPath.ReverseFind('.');
	int nlen = strJPGPath.GetLength();
	strFileName.Format(L"%s", strJPGPath.Mid(nIndex + 1, nIndexEnd - nIndex - 1));

	CString strPicPath;
	GetDlgItemText(IDC_EDIT_DSTPATH, strPicPath);

	USES_CONVERSION;
	char* pJPGPPath = T2A(strJPGPath);
	char* pDstPath = T2A(strPicPath);
	DWORD dw = GetTickCount();
	int nRet = PicConvert::GetInstance()->YuvToAny(pJPGPPath, pDstPath, 1920, 1080, nOutWidth, nOutHeight);
	dw = GetTickCount() - dw;
	TRACE(L"nRet = %d, cost %d.\n", nRet, dw);
}


void CStreamThumbnailDlg::OnBnClickedButtonPictoyuv()
{
	UpdateData();
	CString strJPGPath, strDstPath;
	GetDlgItemText(IDC_EDIT_JPGFILEPATH2, strJPGPath);
	GetDlgItemText(IDC_EDIT_DSTPATH2, strDstPath);

	USES_CONVERSION;
	char* pJPGPPath = T2A(strJPGPath);
	char* pDstPath = T2A(strDstPath);

	CString strXRatio, strYRatio, strWidthRatio, strHeightRatio;
	GetDlgItemText(IDC_EDIT_STEP2_X, strXRatio);
	GetDlgItemText(IDC_EDIT_STEP2_Y, strYRatio);
	GetDlgItemText(IDC_EDIT_STEP2_W, strWidthRatio);
	GetDlgItemText(IDC_EDIT_STEP2_H, strHeightRatio);

	int nOutWidth = GetDlgItemInt(IDC_EDIT_STEP3_W);
	int nOutHeight = GetDlgItemInt(IDC_EDIT_STEP3_H);

	double dX = _ttof(strXRatio);
	double dY = _ttof(strYRatio);
	double dWidth = _ttof(strWidthRatio);
	double dHeight = _ttof(strHeightRatio);

	int nRet = PicConvert::GetInstance()->PicToYuv420p(pJPGPPath, pDstPath, dX, dY, dWidth, dHeight, nOutWidth, nOutHeight);
	TRACE(_T("%d.\n"), nRet);
}


void CStreamThumbnailDlg::OnBnClickedChooseJpg2()
{
	CString filter;
	filter = L"JPG文件(*.jpg)|*.jpg|所有文件(*.*)|*.*||";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
	if (dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.GetPathName();
		SetDlgItemText(IDC_EDIT_JPGFILEPATH2, strPath);
	}
}
