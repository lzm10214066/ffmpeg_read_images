#include "processGif.h"

ProGif::ProGif(int _max_num, double _th, int _timeStep) :max_num(_max_num), th(_th), timeStep(_timeStep)
{
}

int ProGif:: pickGif2Mats(const char* filename, int max_num,vector<Mat> &gif_frames)
{
	AVFormatContext *pFormatCtx;
	int             i, videoindex;
	AVCodecContext  *pCodecCtx=NULL;
	AVCodec         *pCodec;
	AVFrame *pFrame, *pFrameBGR;
	uint8_t *out_buffer;
	AVPacket *packet;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;

	av_register_all();
	pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0)
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}

	videoindex = -1;

	for (i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	}

	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	//avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);  // AV_CODEC_ID_GIF   

	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	pFrame = av_frame_alloc();
	pFrameBGR = av_frame_alloc();

	int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
	out_buffer = (uint8_t *)av_malloc(size);
	avpicture_fill((AVPicture *)pFrameBGR, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//Output Info-----------------------------  
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx, 0, filename, 0);
	printf("-------------------------------------------------\n");

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24,
		SWS_BICUBIC, NULL, NULL, NULL);

	while (gif_frames.size() < max_num)
	{
		int step = timeStep;
		ret = av_read_frame(pFormatCtx, packet);
		if (ret < 0)
		{
			break;
		}

		if (packet->stream_index == videoindex)
		{
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

			if (ret < 0)
			{
				printf("Decode Error.\n");
				return -1;
			}

			if (got_picture)
			{
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameBGR->data, pFrameBGR->linesize);

				Mat show(pFrame->height, pFrame->width, CV_8UC3);
				memcpy(show.data, out_buffer, size);
				gif_frames.push_back(show);
			}
		}

		av_free_packet(packet);

		while (step--)
		{
			ret = av_read_frame(pFormatCtx, packet);
			if (ret < 0)
			{
				break;
			}
			if (packet->stream_index == videoindex)
			{
				ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

				if (ret < 0)
				{
					printf("Decode Error.\n");
					return -1;
				}
			}
			av_free_packet(packet);
		}
	}
	sws_freeContext(img_convert_ctx);
	av_frame_free(&pFrameBGR);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
}


//int ProGif::pickGif2Mats(const char* filename, int max_num, vector<Mat> &gif_frames)
//{
//	AVFormatContext *pFormatCtx;
//	int             i, videoindex;
//	AVCodecContext  *pCodecCtx = NULL;
//	AVCodec         *pCodec;
//	AVFrame *pFrame, *pFrameYUV;
//	uint8_t *out_buffer;
//	AVPacket *packet;
//	int ret, got_picture;
//	struct SwsContext *img_convert_ctx;
//
//
//	av_register_all();
//	pFormatCtx = avformat_alloc_context();
//
//	if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0)
//	{
//		printf("Couldn't open input stream.\n");
//		return -1;
//	}
//
//	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
//	{
//		printf("Couldn't find stream information.\n");
//		return -1;
//	}
//
//	videoindex = -1;
//
//	for (i = 0; i < pFormatCtx->nb_streams; i++)
//	{
//		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
//		{
//			videoindex = i;
//			break;
//		}
//	}
//
//	if (videoindex == -1)
//	{
//		printf("Didn't find a video stream.\n");
//		return -1;
//	}
//
//	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
//	//avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
//	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);  // AV_CODEC_ID_GIF   
//
//	if (pCodec == NULL)
//	{
//		printf("Codec not found.\n");
//		return -1;
//	}
//
//	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
//	{
//		printf("Could not open codec.\n");
//		return -1;
//	}
//
//	pFrame = av_frame_alloc();
//	pFrameYUV = av_frame_alloc();
//
//	int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
//	out_buffer = (uint8_t *)av_malloc(size);
//	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
//
//	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
//
//	//Output Info-----------------------------  
//	printf("--------------- File Information ----------------\n");
//	av_dump_format(pFormatCtx, 0, filename, 0);
//	printf("-------------------------------------------------\n");
//
//	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24,
//		SWS_BICUBIC, NULL, NULL, NULL);
//
//	while (gif_frames.size() < max_num)
//	{
//		int step = timeStep;
//		ret = av_read_frame(pFormatCtx, packet);
//		if (ret < 0)
//		{
//			break;
//		}
//
//		if (packet->stream_index == videoindex)
//		{
//			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
//
//			if (ret < 0)
//			{
//				printf("Decode Error.\n");
//				return -1;
//			}
//
//			if (got_picture)
//			{
//				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
//					pFrameYUV->data, pFrameYUV->linesize);
//
//				Mat show(pFrame->height, pFrame->width, CV_8UC3);
//				memcpy(show.data, out_buffer, size);
//				gif_frames.push_back(show);
//			}
//		}
//
//		av_free_packet(packet);
//
//		while (step--)
//		{
//			ret = av_read_frame(pFormatCtx, packet);
//			if (ret < 0)
//			{
//				break;
//			}
//			//av_free_packet(packet);
//		}
//	}
//	sws_freeContext(img_convert_ctx);
//	av_frame_free(&pFrameYUV);
//	av_frame_free(&pFrame);
//	avcodec_close(pCodecCtx);
//	avformat_close_input(&pFormatCtx);
//}

int  ProGif::showGif(const char* filepath,int repeat)
{
	AVFormatContext *pFormatCtx;
	int             i, videoindex;
	AVCodecContext  *pCodecCtx=NULL;
	AVCodec         *pCodec;
	AVFrame *pFrame, *pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	
	av_register_all();
	pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}

	videoindex = -1;

	for (i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	}

	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	//avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);  // AV_CODEC_ID_GIF   

	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();

	int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
	out_buffer = (uint8_t *)av_malloc(size);
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//Output Info-----------------------------  
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24,
		SWS_BICUBIC, NULL, NULL, NULL);
	int count = 0;
	namedWindow("gif", CV_WINDOW_AUTOSIZE);
	
	while (count < repeat)
	{
		ret = av_read_frame(pFormatCtx, packet);
		if (ret < 0)
		{
			count++;
			av_seek_frame(pFormatCtx, -1, 0 * AV_TIME_BASE, AVSEEK_FLAG_ANY);
		}

		if (packet->stream_index == videoindex)
		{
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

			if (ret < 0)
			{
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture)
			{
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);

				Mat show(pFrame->height, pFrame->width, CV_8UC3);
				memcpy(show.data, out_buffer, size);

				imshow("gif", show);
				waitKey(50);
				printf("Succeed to decode 1 frame!\n");
			}
		}

		av_free_packet(packet);
	}

	//flush decoder  
	//FIX: Flush Frames remained in Codec  
	while (1)
	{
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

		if (ret < 0)
			break;

		if (!got_picture)
			break;

		sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
			pFrameYUV->data, pFrameYUV->linesize);

		Mat show(pFrame->height, pFrame->width, CV_8UC3);
		memcpy(show.data, out_buffer, size);

		imshow("gif", show);
		waitKey(50);
		printf("Flush Decoder: Succeed to decode 1 frame!\n");
	}

	sws_freeContext(img_convert_ctx);
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
}

double ProGif::simiTwoImages(Mat &_imga, Mat &_imgb)
{
	Mat imga = _imga.clone();
	Mat imgb = _imgb.clone();
	if (imga.channels() != 1) cvtColor(imga, imga, CV_BGR2GRAY);
	if (imgb.channels() != 1) cvtColor(imgb, imgb, CV_BGR2GRAY);

	Mat diff;
	absdiff(imga, imgb,diff);
	threshold(diff, diff, 70, 255, THRESH_BINARY);
	//diff /= 8;
	int non_zero = countNonZero(diff);
	double simi = 1-double(non_zero) / (diff.rows*diff.cols);
	return simi;
}

void ProGif::duplicateFilter(vector<Mat> &frames, double th)
{
	vector<Mat> res;
	vector<bool> pass(frames.size(), false);
	for (int i = 0; i < frames.size(); ++i)
	{
		if (pass[i]) continue;
		for (int j = i+1; j < frames.size(); ++j)
		{
			if (pass[j]) continue;
			double simi = simiTwoImages(frames[i], frames[j]);
			if (simi > th) pass[j] = true;
		}
	}
	for (int i = 0; i < frames.size();++i)
	{
		if (!pass[i]) res.push_back(frames[i]);
	}
	frames.clear();
	frames = res;
}

void ProGif::proGif(string &fileName, vector<Mat> &frames)
{
	//double t = (double)getTickCount();

	pickGif2Mats(fileName.c_str(), max_num,frames);

	//t = ((double)getTickCount() - t) / getTickFrequency();
	//cout << "Times passed in ms: " << t * 1000 << endl;
//////////////////////////////////////////////////////////////////
	//t = (double)getTickCount();

	duplicateFilter(frames, th);

	/*t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Times passed in ms: " << t * 1000 << endl;*/
}

void ProGif::readImage(const char *filepath,Mat &img)
{
	AVFormatContext *pFormatCtx;
	int             i, videoindex;
	AVCodecContext  *pCodecCtx = NULL;
	AVCodec         *pCodec;
	AVFrame *pFrame, *pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;

	av_register_all();
	pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)
	{
		printf("Couldn't open input stream.\n");
		return;
	}

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return;
	}

	videoindex = -1;

	for (i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	}

	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return;
	}

	//pCodecCtx = pFormatCtx->streams[videoindex]->codecpar;
	avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);  // AV_CODEC_ID_GIF   

	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return;
	}

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();

	int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
	out_buffer = (uint8_t *)av_malloc(size);
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//Output Info-----------------------------  
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24,
		SWS_BICUBIC, NULL, NULL, NULL);
	int count = 0;
	while (count <= 1)
	{
		ret = av_read_frame(pFormatCtx, packet);
		if (ret < 0)
		{
			count++;
			av_seek_frame(pFormatCtx, -1, 0 * AV_TIME_BASE, AVSEEK_FLAG_ANY);
		}

		if (packet->stream_index == videoindex)
		{
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

			if (ret < 0)
			{
				printf("Decode Error.\n");
				return;
			}
			if (got_picture)
			{
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);

				Mat show(pFrame->height, pFrame->width, CV_8UC3);
				memcpy(show.data, out_buffer, size);

				imshow("gif", show);
				waitKey(50);
				printf("Succeed to decode 1 frame!\n");
			}
		}

		av_free_packet(packet);
	}
}

int  ProGif::pickGifImages(const char *filepath, const string &folder)
{
	AVFormatContext *pFormatCtx;
	int             i, videoindex;
	AVCodecContext  *pCodecCtx = NULL;
	AVCodec         *pCodec;
	AVFrame *pFrame, *pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;

	av_register_all();
	pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}

	videoindex = -1;

	for (i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	}

	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	//avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);  // AV_CODEC_ID_GIF   

	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();

	int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
	out_buffer = (uint8_t *)av_malloc(size);
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//Output Info-----------------------------  
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24,
		SWS_BICUBIC, NULL, NULL, NULL);
	int curFrame = 0;
	while (1)
	{
		ret = av_read_frame(pFormatCtx, packet);
		if (ret < 0)
		{
			return -1;
		}

		if (packet->stream_index == videoindex)
		{
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

			if (ret < 0)
			{
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture)
			{
				if (!pFrame->key_frame) continue;
				curFrame++;
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);

				Mat show(pFrame->height, pFrame->width, CV_8UC3);
				memcpy(show.data, out_buffer, size);

				char srcToSaved[500];
				sprintf(srcToSaved, "%s/%s_%d.png", folder.c_str(), "gif", curFrame);
				imwrite(srcToSaved, show);
				printf("Succeed to decode 1 frame!\n");
			}
		}

		av_free_packet(packet);
	}

	//flush decoder  
	//FIX: Flush Frames remained in Codec  
	while (1)
	{
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

		if (ret < 0)
			break;

		if (!got_picture)
			break;


		sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
			pFrameYUV->data, pFrameYUV->linesize);

		Mat show(pFrame->height, pFrame->width, CV_8UC3);
		memcpy(show.data, out_buffer, size);

		curFrame++;
		char srcToSaved[500];
		sprintf(srcToSaved, "%s/%s_%d.png", folder.c_str(), filepath, curFrame);
		imwrite(srcToSaved, show);
		printf("Flush Decoder: Succeed to decode 1 frame!\n");
	}

	sws_freeContext(img_convert_ctx);
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
}
