
// StreamThumbnail.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CStreamThumbnailApp: 
// �йش����ʵ�֣������ StreamThumbnail.cpp
//

class CStreamThumbnailApp : public CWinApp
{
public:
	CStreamThumbnailApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CStreamThumbnailApp theApp;