#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/time.h"
#ifdef __cplusplus
}
#endif

#ifdef WIN64						 
#pragma comment(lib, "lib\\x64\\avformat.lib")
#pragma comment(lib, "lib\\x64\\avcodec.lib")
#pragma comment(lib, "lib\\x64\\avutil.lib")
#pragma comment(lib, "lib\\x64\\swscale.lib")
#else					
#pragma comment(lib, "lib\\x86\\avformat.lib")
#pragma comment(lib, "lib\\x86\\avcodec.lib");
#pragma comment(lib, "lib\\x86\\avutil.lib")
#pragma comment(lib, "lib\\x86\\swscale.lib")
#endif

class GetStreamThumbnail
{
public:
	GetStreamThumbnail();
	~GetStreamThumbnail();

public:
	static GetStreamThumbnail *Initialize();
	static void Uninitialize();
	static GetStreamThumbnail *GetInstance();

public:
	
	/*
		�������ã���һ��������ȡ����һ֡ͼ�񱣴���ļ���Ŀǰ��PNG��
		������	streamAddr ����ַ 
				strPicPath ͼƬ�洢·��
				nOutWidth  ����ļ��� 0 �������Դ����ͬ
				nOutHeight ����ļ��� 0 �������Դ����ͬ
				nTimeOut   TimeOutʱ��
		����ֵ�� 0���ɹ��� ��0��ʧ�ܡ�
						 -1,����ʧ��	 | -2,��ȡ����Ϣʧ�� | -3,��Ƶ�������� | -4,������ֵʧ�� | 
						 -5,�򿪽�����ʧ�� | -6,���ظ�ʽת��ʧ�� | -7,�򿪱�����ʧ�� | -8,�������д����ʧ�� | -9,д�ļ�ʧ��		
	*/
	int		GetPicFromStream(char* pStreamAddr, char* pPicPath, int nOutWidth = 0, int nOutHeigth = 0, int nTimeOut = 3);

private:
	void		Init();
	void		UnInit();

public:
	static GetStreamThumbnail* m_pInstance;
	SwsContext*		m_pSwsCtx;
	AVFrame*		m_pFrameEnc;
	int64_t			m_i64LastTime;// ��һ����Чʱ�� us
	int64_t			m_i64TimeOut; // Timeoutʱ�� sec
};

