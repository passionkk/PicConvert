
// StreamThumbnailDlg.h : 头文件
//

#pragma once


// CStreamThumbnailDlg 对话框
class CStreamThumbnailDlg : public CDialogEx
{
// 构造
public:
	CStreamThumbnailDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_STREAMTHUMBNAIL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void InitParam();
	void MakeSureDirExist(CString strSavePath);
	afx_msg void OnBnClickedChooseSavepath();
	afx_msg void OnBnClickedOpenStream();

	int m_nPngCount;
	afx_msg void OnClose();
	afx_msg void OnBnClickedChooseJpg();
	afx_msg void OnBnClickedChooseSavebmp();
	afx_msg void OnBnClickedBtnConvert();
	afx_msg void OnBnClickedChooseSave();
	afx_msg void OnBnClickedBtnConvert2();
	afx_msg void OnBnClickedButtonPictoyuv();
	afx_msg void OnBnClickedChooseJpg2();
};
