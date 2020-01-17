#include "stdafx.h"
#include "GetStreamThumbnail.h"

GetStreamThumbnail* GetStreamThumbnail::m_pInstance = NULL;

GetStreamThumbnail::GetStreamThumbnail()
{
	m_pSwsCtx = NULL;
	m_pFrameEnc = NULL;
	m_i64LastTime = 0;
	m_i64TimeOut = 0;
}

GetStreamThumbnail::~GetStreamThumbnail()
{
}

GetStreamThumbnail* GetStreamThumbnail::Initialize()
{
	return GetStreamThumbnail::GetInstance();
}

void GetStreamThumbnail::Uninitialize()
{
	try{
		if (m_pInstance != NULL)
		{
			m_pInstance->UnInit();
			avformat_network_deinit();
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
	catch (...)
	{
		TRACE(L"GetStreamThumbnail::Uninitialize Failed.\n");
	}
}

GetStreamThumbnail* GetStreamThumbnail::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new GetStreamThumbnail;
		m_pInstance->Init();
	}

	return m_pInstance;
}

void GetStreamThumbnail::Init()
{
	try
	{
		av_register_all();
		avcodec_register_all();
		avformat_network_init();
		m_pFrameEnc = av_frame_alloc();
		m_pFrameEnc->format = AV_PIX_FMT_YUVJ420P;
		m_pFrameEnc->width = 0;
		m_pFrameEnc->height = 0;
	}
	catch (...)
	{
		TRACE(L"GetStreamThumbnail::Init Failed.\n");
	}
}

void GetStreamThumbnail::UnInit()
{
	if (m_pFrameEnc)
		av_frame_free(&m_pFrameEnc);
	if (m_pSwsCtx)
		sws_freeContext(m_pSwsCtx);
}

int interrupt_cb(void *ctx)
{
	GetStreamThumbnail *pThis = (GetStreamThumbnail *)ctx;
    if ((av_gettime() - pThis->m_i64LastTime) > pThis->m_i64TimeOut)
	{
		//³¬Ê±ÍË³ö  
		printf("TimeOut.\n");
		return -1;
	}
    return 0;
}

int	GetStreamThumbnail::GetPicFromStream(char* pStreamAddr, char* pPicPath, int nOutWidth, int nOutHeigth, int nTimeOut)
{
	int nRet = 0;
	AVFormatContext* pFormatCtx = avformat_alloc_context();
	if (pFormatCtx)
	{
		pFormatCtx->flags |= AVFMT_FLAG_NONBLOCK;
		pFormatCtx->interrupt_callback.callback = interrupt_cb;
		pFormatCtx->interrupt_callback.opaque = (void *)this;
		m_i64TimeOut = nTimeOut * AV_TIME_BASE;
		m_i64LastTime = av_gettime();
		AVDictionary* pOp = NULL;
		av_dict_set(&pOp, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		av_dict_set(&pOp, "rtsp_transport", "tcp", 0);
		if (avformat_open_input(&pFormatCtx, pStreamAddr, NULL, &pOp) < 0)
		{
			nRet = -1;
		}
		if (nRet == 0 && avformat_find_stream_info(pFormatCtx, NULL) < 0)
		{
			nRet = -2;
		}
		if (nRet == 0)
		{
			int nVideoIndex = -1;
			for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
			{
				AVStream* pStream = pFormatCtx->streams[i];
				enum AVMediaType eStreamType = pStream->codecpar->codec_type;
				if (eStreamType == AVMEDIA_TYPE_VIDEO)
				{
					nVideoIndex = i;
					break;
				}
			}
			if (nVideoIndex >= 0)
			{
				AVCodecContext* pCodecCtx = pFormatCtx->streams[nVideoIndex]->codec;
				if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[nVideoIndex]->codecpar) < 0)
				{
					avcodec_free_context(&pCodecCtx);
					nRet = -4;
				}
				else
				{
					AVDictionary* pOpts = NULL;
					av_dict_set(&pOpts, "refcounted_frames", "1", 0);
					AVCodec* pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
					pCodecCtx->codec_id = pCodec->id;
					if (avcodec_open2(pCodecCtx, pCodec, pOpts ? &pOpts : NULL) < 0)
					{
						avcodec_free_context(&pCodecCtx);
						nRet = -5;
					}
					else
					{
						AVPacket packet;
						av_init_packet(&packet);
						AVFrame* pFrame = av_frame_alloc();
						int nError = 0;
						bool bGetOne = false;
						do 
						{
							m_i64LastTime = av_gettime();
							nError = av_read_frame(pFormatCtx, &packet);
							if (0 == nError)
							{
								if (0 == avcodec_send_packet(pCodecCtx, &packet))
								{
									if (0 == avcodec_receive_frame(pCodecCtx, pFrame))
									{
										// AV_PICTURE_TYPE_I = 1
										bGetOne = true;
									}
								}
							}
							av_packet_unref(&packet);
						} while (nError == 0 && !bGetOne);
						if (bGetOne)
						{
							//±àÂëÒ»Ö¡
							AVFormatContext* pEncFmtCtx = NULL;
							int nRetFmt = avformat_alloc_output_context2(&pEncFmtCtx, NULL, "image2", pPicPath);
							pEncFmtCtx->duration = 1000;

							AVStream* pEncVS = avformat_new_stream(pEncFmtCtx, NULL);
							pEncVS->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
							pEncVS->codecpar->codec_id = AV_CODEC_ID_MJPEG;

							AVCodec* pEncodec = avcodec_find_encoder(pEncVS->codecpar->codec_id);
							AVCodecContext* pCodecCtxEnc = avcodec_alloc_context3(pEncodec);
							pCodecCtxEnc->chroma_sample_location = AVCHROMA_LOC_LEFT;
							pCodecCtxEnc->codec_type = AVMEDIA_TYPE_VIDEO;

							if (nOutWidth > 0)
								pCodecCtxEnc->width = nOutWidth;
							else
								pCodecCtxEnc->width = pCodecCtx->width;
							if (nOutHeigth > 0)
								pCodecCtxEnc->height = nOutHeigth;
							else
								pCodecCtxEnc->height = pCodecCtx->height;

							pCodecCtxEnc->pix_fmt = AV_PIX_FMT_YUVJ420P;
							pCodecCtxEnc->codec_type = AVMEDIA_TYPE_VIDEO;
							pCodecCtxEnc->gop_size = 15;
							pCodecCtxEnc->time_base.num = 1;
							pCodecCtxEnc->time_base.den = 25;
							avcodec_parameters_from_context(pEncVS->codecpar, pCodecCtxEnc);

							AVPacket packet;
							av_init_packet(&packet);
							AVDictionary* pOpts = NULL;
							av_dict_set(&pOpts, "refcounted_frames", "1", 0);
							if (0 == avcodec_open2(pCodecCtxEnc, pEncodec, &pOpts))
							{
								//Write Header  
								avformat_write_header(pEncFmtCtx, NULL);

								m_pSwsCtx = sws_getCachedContext(m_pSwsCtx, pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
																 pCodecCtxEnc->width, pCodecCtxEnc->height, pCodecCtxEnc->pix_fmt, 0, NULL, NULL, NULL);

								int nRst = 0;
								if (m_pSwsCtx)
								{
									if (m_pFrameEnc->width != pCodecCtxEnc->width || m_pFrameEnc->height != pCodecCtxEnc->height)
									{
										av_frame_free(&m_pFrameEnc);
										m_pFrameEnc = av_frame_alloc();
										m_pFrameEnc->format = AV_PIX_FMT_YUVJ420P;
										m_pFrameEnc->width = pCodecCtxEnc->width;
										m_pFrameEnc->height = pCodecCtxEnc->height;
										av_frame_get_buffer(m_pFrameEnc, 1);
									}
									nRst = sws_scale(m_pSwsCtx, pFrame->data, pFrame->linesize, 0, pFrame->height, m_pFrameEnc->data, m_pFrameEnc->linesize);
									if (nRst > 0)
										nRst = avcodec_send_frame(pCodecCtxEnc, m_pFrameEnc);
									else
										nRet = -6;
								}
								else
									nRst = avcodec_send_frame(pCodecCtxEnc, pFrame);
								if (nRst == 0 && nRet != -6)
								{
									if (0 == avcodec_receive_packet(pCodecCtxEnc, &packet))
									{

										if (0 != av_interleaved_write_frame(pEncFmtCtx, &packet))
											nRet = -9;
										av_packet_unref(&packet);
										avcodec_close(pCodecCtxEnc);
									}
								}
								if (nRst != 0 && nRet != -6)
									nRet = -8;
								av_write_trailer(pEncFmtCtx);
							}
							else
								nRet = -7;
							avcodec_free_context(&pCodecCtxEnc);
						}
						av_frame_free(&pFrame);
						avcodec_close(pCodecCtx);
					}
				}
			}
			else
			{
				nRet = -3;
			}
		}
		avformat_close_input(&pFormatCtx);
	}

	return nRet;
}