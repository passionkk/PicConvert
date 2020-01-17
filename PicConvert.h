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
		函数作用：将JPG文件转为bmp文件（BGR24）
		参数：	pSrcFile jpg文件路径
				pDstFile bmp文件存储路径
				nOutWidth  输出文件宽 0 则输出与源宽相同
				nOutHeight 输出文件高 0 则输出与源高相同
		返回值： 0，成功； 非0，失败。
						-1,打开流失败		| -2,获取流信息失败 | -3,视频流不存在 | -4,参数赋值失败 |
						-5,打开解码器失败 | -6,像素格式转换失败 | -7,打开编码器失败 | -8,向编码器写数据失败 | -9,写文件失败
	*/
	int	  PicToBmp24(const char* pSrcFile, const char* pDstFile, int nOutWidth = 0, int nOutHeight = 0);

	/*
		函数作用：将YUV文件转为其他文件bmp(BGR24)/png(BGR24)/jpg(yuvj420p)
		参数：	pSrcFile 源文件路径
		pDstFile	目标文件存储路径
		nSrcWidth	源yuv文件宽
		nSrcHeight	源yuv文件高
		nOutWidth	输出文件宽 0 则输出与源宽相同
		nOutHeight	输出文件高 0 则输出与源高相同
		返回值：		0，成功； 非0，失败。
		-1,打开流失败		| -2,获取流信息失败 | -3,视频流不存在 | -4,参数赋值失败 |
		-5,打开解码器失败 | -6,像素格式转换失败 | -7,打开编码器失败 | -8,向编码器写数据失败 | -9,写文件失败
	*/
	int	  YuvToAny(const char* pSrcFile, const char* pDstFile,
				   const int& nSrcWidth, const int& nSrcHeight,
				   int nOutWidth = 0, int nOutHeight = 0);

	/*函数作用：将jpg文件转为YUV文件.
		具体描述：输入文件 == 》pRect1分辨率YUV图像 == 》截取pRect1的部分pRect2缩放为pRect3分别率的YUV图像。
		参数：
		pSrcFile	源文件路径
		pDstFile	目标文件存储路径
		dXRatio		裁剪区域的【x坐标】相对于原图像的【宽】比例 如0.5则从原图像宽为一半的位置裁剪
		dYRatio		裁剪区域的【y坐标】相对于原图像的【高】比例 
		dWidthRatio	裁剪区域的【宽度】相当于原图像的【宽】比例 
		dXRatio		裁剪区域的【高度】相对于原图像的【高】比例
		返回值：		0，成功； 非0，失败。
		- 1, 打开流失败 | -2, 获取流信息失败 | -3, 视频流不存在 | -4, 参数赋值失败 |
		-5, 打开解码器失败 | -6 查找解码器失败 | -7 创建文件失败 | -8 获取filter失败 |
		-9 filterIO申请失败 | -10 - 11 创建FilterGraph失败 | -12 FilterGraph解析失败
		- 13 FilgerGraph配置失败 | -14 向Filter中添加Frame失败 | -15 从filter中获取数据失败
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

