#include "PicConvert.h"

PicConvert* PicConvert::m_pInstance = NULL;

PicConvert::PicConvert()
{
	m_pSwsCtx = NULL;
	m_pFrameEnc = NULL;
}

PicConvert::~PicConvert()
{
}

PicConvert* PicConvert::Initialize()
{
	return PicConvert::GetInstance();
}

void PicConvert::Uninitialize()
{
	try
	{
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
		//TRACE(L"GetStreamThumbnail::Uninitialize Failed.\n");
	}
}

PicConvert* PicConvert::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new PicConvert;
		m_pInstance->Init();
	}

	return m_pInstance;
}

void PicConvert::Init()
{
	try
	{
		av_register_all();
		avcodec_register_all();
		avfilter_register_all();
		m_pFrameEnc = av_frame_alloc();
		m_pFrameEnc->format = AV_PIX_FMT_RGB24;
		m_pFrameEnc->width = 0;
		m_pFrameEnc->height = 0;
	}
	catch (...)
	{
		//TRACE(L"GetStreamThumbnail::Init Failed.\n");
	}
}

void PicConvert::UnInit()
{
	if (m_pFrameEnc)
		av_frame_free(&m_pFrameEnc);
	if (m_pSwsCtx)
		sws_freeContext(m_pSwsCtx);
}

int PicConvert::PicToBmp24(const char* pSrcFile, const char* pDstFile, int nOutWidth, int nOutHeight)
{
	int nRet = 0;
	AVFormatContext* pFormatCtx = avformat_alloc_context();
	if (pFormatCtx)
	{
		AVDictionary* pOp = NULL;
		pFormatCtx->flags |= AVFMT_FLAG_KEEP_SIDE_DATA;
		av_dict_set(&pOp, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		if (avformat_open_input(&pFormatCtx, pSrcFile, NULL, &pOp) < 0)
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
					if (pCodec != NULL)
					{
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
								nError = av_read_frame(pFormatCtx, &packet);
								if (0 == nError)
								{
									if (0 == avcodec_send_packet(pCodecCtx, &packet))
									{
										if (0 == avcodec_receive_frame(pCodecCtx, pFrame))
										{
											bGetOne = true;
										}
									}
								}
								av_packet_unref(&packet);
							} while (nError == 0 && !bGetOne); if (bGetOne)
							{
								//±àÂëÒ»Ö¡
								AVFormatContext* pEncFmtCtx = NULL;
								int nRetFmt = avformat_alloc_output_context2(&pEncFmtCtx, NULL, "image2", pDstFile);
								pEncFmtCtx->duration = 1000;
								AVStream* pEncVS = avformat_new_stream(pEncFmtCtx, NULL);
								pEncVS->codecpar->codec_id = av_guess_codec(pEncFmtCtx->oformat, NULL, pEncFmtCtx->filename, NULL, AVMEDIA_TYPE_VIDEO);
								AVCodec* pEncodec = avcodec_find_encoder(pEncVS->codecpar->codec_id);
								AVCodecContext* pCodecCtxEnc = avcodec_alloc_context3(pEncodec);

								if (nOutWidth > 0)
									pCodecCtxEnc->width = nOutWidth;
								else
									pCodecCtxEnc->width = pCodecCtx->width;
								if (nOutHeight > 0)
									pCodecCtxEnc->height = nOutHeight;
								else
									pCodecCtxEnc->height = pCodecCtx->height;

								pCodecCtxEnc->chroma_sample_location = AVCHROMA_LOC_CENTER;
								pCodecCtxEnc->pix_fmt = AV_PIX_FMT_BGR24;
								pCodecCtxEnc->codec_type = AVMEDIA_TYPE_VIDEO;
								pCodecCtxEnc->gop_size = 15;
								pCodecCtxEnc->time_base.num = 1;
								pCodecCtxEnc->time_base.den = 25;
								pCodecCtxEnc->sample_aspect_ratio.den = 1;
								pCodecCtxEnc->sample_aspect_ratio.num = 1;
								pCodecCtxEnc->bits_per_raw_sample = 8;
								pCodecCtxEnc->framerate = av_make_q(25, 1);
								pEncVS->avg_frame_rate = av_make_q(25, 1);

								AVPacket packet;
								av_init_packet(&packet);
								AVDictionary* pOpts = NULL;
								av_dict_set(&pOpts, "refcounted_frames", "1", 0);
								int nOpen = avcodec_open2(pCodecCtxEnc, pEncodec, &pOpts);
								if (0 == nOpen)
								{
									//Write Header  
									avformat_write_header(pEncFmtCtx, NULL);

									m_pSwsCtx = sws_getCachedContext(m_pSwsCtx, pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
										pCodecCtxEnc->width, pCodecCtxEnc->height, pCodecCtxEnc->pix_fmt, 0, NULL, NULL, NULL);

									int nRst = 0;
									if (m_pSwsCtx)
									{
										if (m_pFrameEnc->width != pCodecCtxEnc->width ||
											m_pFrameEnc->height != pCodecCtxEnc->height ||
											m_pFrameEnc->format != (int)pCodecCtxEnc->pix_fmt)
										{
											av_frame_free(&m_pFrameEnc);
											m_pFrameEnc = av_frame_alloc();
											m_pFrameEnc->format = AV_PIX_FMT_RGB24;
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
					else
					{
						nRet = -10;
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

int	PicConvert::YuvToAny(const char* pSrcFile, const char* pDstFile,
	const int& nSrcWidth, const int& nSrcHeight,
	int nOutWidth, int nOutHeight)
{
	int nRet = 0;
	AVFormatContext* pFormatCtx = avformat_alloc_context();
	if (pFormatCtx)
	{
		AVDictionary* pOp = NULL;
		pFormatCtx->flags |= AVFMT_FLAG_KEEP_SIDE_DATA;
		av_dict_set(&pOp, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		char szVideoSize[20];
		memset(szVideoSize, 0, sizeof(szVideoSize) / sizeof(char));
		sprintf(szVideoSize, "%dx%d", nSrcWidth, nSrcHeight);
		av_dict_set(&pOp, "video_size", szVideoSize, 1);
		if (avformat_open_input(&pFormatCtx, pSrcFile, NULL, &pOp) < 0)
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
					if (pCodec != NULL)
					{
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
								nError = av_read_frame(pFormatCtx, &packet);
								if (0 == nError)
								{
									if (0 == avcodec_send_packet(pCodecCtx, &packet))
									{
										if (0 == avcodec_receive_frame(pCodecCtx, pFrame))
										{
											bGetOne = true;
										}
									}
								}
								av_packet_unref(&packet);
							} while (nError == 0 && !bGetOne); if (bGetOne)
							{
								//±àÂëÒ»Ö¡
								AVFormatContext* pEncFmtCtx = NULL;
								int nRetFmt = avformat_alloc_output_context2(&pEncFmtCtx, NULL, "image2", pDstFile);
								pEncFmtCtx->duration = 1000;
								AVStream* pEncVS = avformat_new_stream(pEncFmtCtx, NULL);
								pEncVS->codecpar->codec_id = av_guess_codec(pEncFmtCtx->oformat, NULL, pEncFmtCtx->filename, NULL, AVMEDIA_TYPE_VIDEO);
								AVCodec* pEncodec = avcodec_find_encoder(pEncVS->codecpar->codec_id);
								AVCodecContext* pCodecCtxEnc = avcodec_alloc_context3(pEncodec);

								if (nOutWidth > 0)
									pCodecCtxEnc->width = nOutWidth;
								else
									pCodecCtxEnc->width = pCodecCtx->width;
								if (nOutHeight > 0)
									pCodecCtxEnc->height = nOutHeight;
								else
									pCodecCtxEnc->height = pCodecCtx->height;

								//avcodec_default_get_format
								pCodecCtxEnc->pix_fmt = *pEncodec->pix_fmts;
								pCodecCtxEnc->codec_type = AVMEDIA_TYPE_VIDEO;
								pCodecCtxEnc->gop_size = 15;
								pCodecCtxEnc->time_base.num = 1;
								pCodecCtxEnc->time_base.den = 25;
								pCodecCtxEnc->sample_aspect_ratio.den = 1;
								pCodecCtxEnc->sample_aspect_ratio.num = 1;
								pCodecCtxEnc->framerate = av_make_q(25, 1);
								pEncVS->avg_frame_rate = av_make_q(25, 1);

								AVPacket packet;
								av_init_packet(&packet);
								AVDictionary* pOpts = NULL;
								av_dict_set(&pOpts, "refcounted_frames", "1", 0);
								int nOpen = avcodec_open2(pCodecCtxEnc, pEncodec, &pOpts);
								if (0 == nOpen)
								{
									//Write Header  
									avformat_write_header(pEncFmtCtx, NULL);

									m_pSwsCtx = sws_getCachedContext(m_pSwsCtx, pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
										pCodecCtxEnc->width, pCodecCtxEnc->height, pCodecCtxEnc->pix_fmt, 0, NULL, NULL, NULL);

									int nRst = 0;
									if (m_pSwsCtx)
									{
										if (m_pFrameEnc->width != pCodecCtxEnc->width ||
											m_pFrameEnc->height != pCodecCtxEnc->height ||
											m_pFrameEnc->format != (int)pCodecCtxEnc->pix_fmt)
										{
											av_frame_free(&m_pFrameEnc);
											m_pFrameEnc = av_frame_alloc();
											m_pFrameEnc->format = pCodecCtxEnc->pix_fmt;
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
					else
					{
						nRet = -10;
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

int	PicConvert::PicToYuv420p(const char* pSrcFile, const char* pDstFile,
	double dXRatio, double dYRatio, double dWidthRatio, double dHeightRatio,
	int nOutWidth, int nOutHeight)
{
	int nRet = 0;
	AVFormatContext* pFormatCtx = avformat_alloc_context();
	if (pFormatCtx)
	{
		AVDictionary* pOp = NULL;
		pFormatCtx->flags |= AVFMT_FLAG_KEEP_SIDE_DATA;
		av_dict_set(&pOp, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		if (avformat_open_input(&pFormatCtx, pSrcFile, NULL, &pOp) < 0)
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
					if (pCodec != NULL)
					{
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
								nError = av_read_frame(pFormatCtx, &packet);
								if (0 == nError)
								{
									if (0 == avcodec_send_packet(pCodecCtx, &packet))
									{
										if (0 == avcodec_receive_frame(pCodecCtx, pFrame))
										{
											bGetOne = true;
										}
									}
								}
								av_packet_unref(&packet);
							} while (nError == 0 && !bGetOne);
							if (bGetOne)
							{
								nRet = CropScale(pFrame, dXRatio* pFrame->width, dYRatio*pFrame->height,
									dWidthRatio*pFrame->width, dHeightRatio* pFrame->height,
									nOutWidth, nOutHeight);
								if (nRet == 0)
								{
									FILE* fp_yuv2 = fopen(pDstFile, "wb");
									if (fp_yuv2)
									{
										for (int i = 0; i < m_pFrameEnc->height; i++){
											fwrite(m_pFrameEnc->data[0] + m_pFrameEnc->linesize[0] * i, 1, m_pFrameEnc->width, fp_yuv2);
										}
										for (int i = 0; i < m_pFrameEnc->height / 2; i++){
											fwrite(m_pFrameEnc->data[1] + m_pFrameEnc->linesize[1] * i, 1, m_pFrameEnc->width / 2, fp_yuv2);
										}
										for (int i = 0; i < m_pFrameEnc->height / 2; i++){
											fwrite(m_pFrameEnc->data[2] + m_pFrameEnc->linesize[2] * i, 1, m_pFrameEnc->width / 2, fp_yuv2);
										}
										fclose(fp_yuv2);
									}
									else
									{
										nRet = -7;
									}
								}

							}

							av_frame_free(&pFrame);
						}
					}
					else
					{
						nRet = -6;
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
	return 0;
}

int	 PicConvert::PicToYuv420p(const char* pSrcFile, const char* pDstFile,
	Fraction stX, Fraction stY, Fraction stWidth, Fraction stHeight,
	int nOutWidth, int nOutHeight)
{
	if (stX.nDen == 0 || stY.nDen == 0 || stWidth.nDen == 0 || stHeight.nDen == 0)
		return -1;
	double dXRatio = stX.nNum / stX.nDen;
	double dYRatio = stY.nNum / stY.nDen;
	double dWidthRatio = stWidth.nNum / stWidth.nDen;
	double dHeightRatio = stHeight.nNum / stHeight.nDen;

	return PicToYuv420p(pSrcFile, pDstFile, dXRatio, dYRatio, dWidthRatio, dHeightRatio, nOutWidth, nOutHeight);
}

int	PicConvert::CropScale(AVFrame* pFrame, int nCropX, int nCropY, int nCropWidth, int nCropHeight, int nOutWidth, int nOutHeight)
{
	int nRet = 0;

	//±àÂëÒ»Ö¡
	AVPixelFormat pixFmtOut = AV_PIX_FMT_YUV420P;

	if (m_pFrameEnc->width != nOutWidth ||
		m_pFrameEnc->height != nOutHeight ||
		m_pFrameEnc->format != pixFmtOut)
	{
		av_frame_free(&m_pFrameEnc);
		m_pFrameEnc = av_frame_alloc();
		m_pFrameEnc->format = pixFmtOut;
		m_pFrameEnc->width = nOutWidth;
		m_pFrameEnc->height = nOutHeight;
		av_frame_get_buffer(m_pFrameEnc, 1);
	}

	//filterµÇ³¡
	AVFilterContext* pBufferSinkCtx = NULL;
	AVFilterContext* pBufferSrcCtx = NULL;
	AVFilterInOut* pOutputs = NULL;
	AVFilterInOut* pInputs = NULL;
	AVFilterGraph* pFilterGraph = NULL;
	do
	{
		AVFilter* pBufferSrc = avfilter_get_by_name("buffer");
		AVFilter* pBufferSink = avfilter_get_by_name("buffersink");
		if (pBufferSink == NULL || pBufferSink == NULL)
		{
			nRet = -8;
			break;
		}
		pOutputs = avfilter_inout_alloc();
		pInputs = avfilter_inout_alloc();

		if (pOutputs == NULL || pInputs == NULL)
		{
			nRet = -9;
			break;
		}

		AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };

		char szArgs[512] = { 0 };
#ifdef _WIN32
		_snprintf(szArgs, sizeof(szArgs),
			"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
			pFrame->width, pFrame->height, pFrame->format,
			1, 25, 1, 1);
#else
		snprintf(szArgs, sizeof(szArgs),
			"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
			pFrame->width, pFrame->height, pFrame->format,
			1, 25, 1, 1);
#endif
		pFilterGraph = avfilter_graph_alloc();
		int ret = avfilter_graph_create_filter(&pBufferSrcCtx, pBufferSrc, "in",
			szArgs, NULL, pFilterGraph);
		if (ret < 0) {
			nRet = -10;
			break;
			//printf("Cannot create buffer source\n");
		}
		//×¢Òâbuffersrc ¸ú buffer sinkµÄÊÍ·Å¡£
		/* buffer video sink: to terminate the filter chain. */
		AVBufferSinkParams* pBufferSinkParams = av_buffersink_params_alloc();
		pBufferSinkParams->pixel_fmts = pix_fmts;
		ret = avfilter_graph_create_filter(&pBufferSinkCtx, pBufferSink, "out",
			NULL, pBufferSinkParams, pFilterGraph);
		av_free(pBufferSinkParams);
		if (ret < 0) {
			nRet = -11;
			break;
			printf("Cannot create buffer sink\n");
		}

		/* Endpoints for the filter graph. */
		pOutputs->name = av_strdup("in");
		pOutputs->filter_ctx = pBufferSrcCtx;
		pOutputs->pad_idx = 0;
		pOutputs->next = NULL;

		pInputs->name = av_strdup("out");
		pInputs->filter_ctx = pBufferSinkCtx;
		pInputs->pad_idx = 0;
		pInputs->next = NULL;

		char filter_descr[256] = { 0 };
#ifdef _WIN32
		_snprintf(filter_descr, sizeof(filter_descr),
			"crop=w=%d:h=%d:x=%d:y=%d,scale=%d:%d,format=pix_fmts=yuv420p",
			nCropWidth, nCropHeight, nCropX, nCropY, nOutWidth, nOutHeight);
#else
		snprintf(filter_descr, sizeof(filter_descr),
			"crop=w=%d:h=%d:x=%d:y=%d,scale=%d:%d,format=pix_fmts=yuv420p",
			nCropWidth, nCropHeight, nCropX, nCropY, nOutWidth, nOutHeight);
#endif
		//const char *filter_descr = "crop=w=800:h=800:x=200:y=200,scale=1920:1080";
		if ((ret = avfilter_graph_parse_ptr(pFilterGraph, filter_descr,
			&pInputs, &pOutputs, NULL)) < 0)
		{
			nRet = -12;
			break;
		}

		if ((ret = avfilter_graph_config(pFilterGraph, NULL)) < 0)
		{
			nRet = -13;
			break;
		}

		if (av_buffersrc_add_frame(pBufferSrcCtx, pFrame) < 0)
		{
			//printf("Error while add frame.\n");
			nRet = -14;
			break;
		}

		/* get one picture from the filtergraph */
		ret = av_buffersink_get_frame(pBufferSinkCtx, m_pFrameEnc);
		if (ret < 0)
		{
			nRet = -15;
			break;
		}
	}while (0);

	if (pBufferSinkCtx)
		avfilter_free(pBufferSinkCtx);
	if (pBufferSrcCtx)
		avfilter_free(pBufferSrcCtx);
	if (pOutputs)
		avfilter_inout_free(&pOutputs);
	if (pInputs)
		avfilter_inout_free(&pInputs);
	if (pFilterGraph)
		avfilter_graph_free(&pFilterGraph);

	return nRet;
}