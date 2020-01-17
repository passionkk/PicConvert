
// StreamThumbnailDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CStreamThumbnailDlg �Ի���



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


// CStreamThumbnailDlg ��Ϣ�������

BOOL CStreamThumbnailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	GetStreamThumbnail::Initialize();

	InitParam();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CStreamThumbnailDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	//�ж��Ƿ���ڣ����򴴽�
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
	sInfo.lpszTitle = _T("��ѡ��������ͼ�洢·��");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// ��ʾ�ļ���ѡ��Ի���  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// ȡ���ļ�����  
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
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	GetStreamThumbnail::Uninitialize();
	PicConvert::Uninitialize();

	CDialogEx::OnClose();
}


void CStreamThumbnailDlg::OnBnClickedChooseJpg()
{
	CString filter;
	filter = L"JPG�ļ�(*.jpg)|*.jpg|�����ļ�(*.*)|*.*||";

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
	sInfo.lpszTitle = _T("��ѡ��BMP�ļ��洢·��");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// ��ʾ�ļ���ѡ��Ի���  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// ȡ���ļ�����  
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
	sInfo.lpszTitle = _T("��ѡ��Ŀ���ļ��洢·��");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// ��ʾ�ļ���ѡ��Ի���  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// ȡ���ļ�����  
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
		AfxMessageBox(_T("���Դ·����Ŀ��·���Ƿ���ȷ"));
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
	filter = L"JPG�ļ�(*.jpg)|*.jpg|�����ļ�(*.*)|*.*||";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
	if (dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.GetPathName();
		SetDlgItemText(IDC_EDIT_JPGFILEPATH2, strPath);
	}
}
