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
		函数作用：打开一个流并获取其中一帧图像保存成文件（目前是PNG）
		参数：	streamAddr 流地址 
				strPicPath 图片存储路径
				nOutWidth  输出文件宽 0 则输出与源宽相同
				nOutHeight 输出文件高 0 则输出与源高相同
				nTimeOut   TimeOut时长
		返回值： 0，成功； 非0，失败。
						 -1,打开流失败	 | -2,获取流信息失败 | -3,视频流不存在 | -4,参数赋值失败 | 
						 -5,打开解码器失败 | -6,像素格式转换失败 | -7,打开编码器失败 | -8,向编码器写数据失败 | -9,写文件失败		
	*/
	int		GetPicFromStream(char* pStreamAddr, char* pPicPath, int nOutWidth = 0, int nOutHeigth = 0, int nTimeOut = 3);

private:
	void		Init();
	void		UnInit();

public:
	static GetStreamThumbnail* m_pInstance;
	SwsContext*		m_pSwsCtx;
	AVFrame*		m_pFrameEnc;
	int64_t			m_i64LastTime;// 上一次有效时间 us
	int64_t			m_i64TimeOut; // Timeout时长 sec
};

