#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#ifdef __cplusplus
}
#endif

#ifdef WIN64						 
#pragma comment(lib, "lib\\x64\\avformat.lib")
#pragma comment(lib, "lib\\x64\\avcodec.lib")
#pragma comment(lib, "lib\\x64\\avutil.lib")
#pragma comment(lib, "lib\\x64\\swscale.lib")
#pragma comment(lib, "lib\\x64\\avfilter.lib")
#else					
#pragma comment(lib, "lib\\x86\\avformat.lib")
#pragma comment(lib, "lib\\x86\\avcodec.lib")
#pragma comment(lib, "lib\\x86\\avutil.lib")
#pragma comment(lib, "lib\\x86\\swscale.lib")
#pragma comment(lib, "lib\\x86\\avfilter.lib")
#endif


typedef struct tagFraction
{
	int nNum;
	int nDen;
}Fraction;

class PicConvert
{
public:
	PicConvert();
	~PicConvert();

public:
	static PicConvert *Initialize();
	static void Uninitialize();
	static PicConvert *GetInstance();

	/*
		�������ã���JPG�ļ�תΪbmp�ļ���BGR24��
		������	pSrcFile jpg�ļ�·��
				pDstFile bmp�ļ��洢·��
				nOutWidth  ����ļ��� 0 �������Դ����ͬ
				nOutHeight ����ļ��� 0 �������Դ����ͬ
		����ֵ�� 0���ɹ��� ��0��ʧ�ܡ�
						-1,����ʧ��		| -2,��ȡ����Ϣʧ�� | -3,��Ƶ�������� | -4,������ֵʧ�� |
						-5,�򿪽�����ʧ�� | -6,���ظ�ʽת��ʧ�� | -7,�򿪱�����ʧ�� | -8,�������д����ʧ�� | -9,д�ļ�ʧ��
	*/
	int	  PicToBmp24(const char* pSrcFile, const char* pDstFile, int nOutWidth = 0, int nOutHeight = 0);

	/*
		�������ã���YUV�ļ�תΪ�����ļ�bmp(BGR24)/png(BGR24)/jpg(yuvj420p)
		������	pSrcFile Դ�ļ�·��
		pDstFile	Ŀ���ļ��洢·��
		nSrcWidth	Դyuv�ļ���
		nSrcHeight	Դyuv�ļ���
		nOutWidth	����ļ��� 0 �������Դ����ͬ
		nOutHeight	����ļ��� 0 �������Դ����ͬ
		����ֵ��		0���ɹ��� ��0��ʧ�ܡ�
		-1,����ʧ��		| -2,��ȡ����Ϣʧ�� | -3,��Ƶ�������� | -4,������ֵʧ�� |
		-5,�򿪽�����ʧ�� | -6,���ظ�ʽת��ʧ�� | -7,�򿪱�����ʧ�� | -8,�������д����ʧ�� | -9,д�ļ�ʧ��
	*/
	int	  YuvToAny(const char* pSrcFile, const char* pDstFile,
				   const int& nSrcWidth, const int& nSrcHeight,
				   int nOutWidth = 0, int nOutHeight = 0);

	/*�������ã���jpg�ļ�תΪYUV�ļ�.
		���������������ļ� == ��pRect1�ֱ���YUVͼ�� == ����ȡpRect1�Ĳ���pRect2����ΪpRect3�ֱ��ʵ�YUVͼ��
		������
		pSrcFile	Դ�ļ�·��
		pDstFile	Ŀ���ļ��洢·��
		dXRatio		�ü�����ġ�x���꡿�����ԭͼ��ġ������� ��0.5���ԭͼ���Ϊһ���λ�òü�
		dYRatio		�ü�����ġ�y���꡿�����ԭͼ��ġ��ߡ����� 
		dWidthRatio	�ü�����ġ���ȡ��൱��ԭͼ��ġ������� 
		dXRatio		�ü�����ġ��߶ȡ������ԭͼ��ġ��ߡ�����
		����ֵ��		0���ɹ��� ��0��ʧ�ܡ�
		- 1, ����ʧ�� | -2, ��ȡ����Ϣʧ�� | -3, ��Ƶ�������� | -4, ������ֵʧ�� |
		-5, �򿪽�����ʧ�� | -6 ���ҽ�����ʧ�� | -7 �����ļ�ʧ�� | -8 ��ȡfilterʧ�� |
		-9 filterIO����ʧ�� | -10 - 11 ����FilterGraphʧ�� | -12 FilterGraph����ʧ��
		- 13 FilgerGraph����ʧ�� | -14 ��Filter�����Frameʧ�� | -15 ��filter�л�ȡ����ʧ��
	*/
	int	 PicToYuv420p(const char* pSrcFile, const char* pDstFile,
						double dXRatio, double dYRatio, double dWidthRatio, double dHeightRatio,
						int nOutWidth = 0, int nOutHeight = 0);

	int	 PicToYuv420p(const char* pSrcFile, const char* pDstFile, 
						Fraction stX, Fraction stY, Fraction stWidth, Fraction stHeight,
						int nOutWidth = 0, int nOutHeight = 0);


private:
	void Init();
	void UnInit();

	int	CropScale(AVFrame* pFrame, int nX, int nY, int nWidth, int nHeight, int nOutWidth, int nOutHeight);

public:
	static PicConvert* m_pInstance;
	SwsContext*		m_pSwsCtx;
	AVFrame*		m_pFrameEnc;
};

