
// StreamThumbnailDlg.h : ͷ�ļ�
//

#pragma once


// CStreamThumbnailDlg �Ի���
class CStreamThumbnailDlg : public CDialogEx
{
// ����
public:
	CStreamThumbnailDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_STREAMTHUMBNAIL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
