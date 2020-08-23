// gif_decode_encode_test.cpp : 定义控制台应用程序的入口点。  
//
#include <stdio.h>  

#define __STDC_CONSTANT_MACROS  

#ifdef _WIN32  
//Windows  
extern "C"
{
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h"  
#include "libswscale/swscale.h"  
};
#else  
//Linux...  
#ifdef __cplusplus  
extern "C"
{
#endif  
#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>  
#include <libswscale/swscale.h>  
#ifdef __cplusplus  
};
#endif  
#endif  

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	AVFormatContext *pFormatCtx;
	int             i, videoindex;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame *pFrame, *pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	char filepath[] = "tt.mp4";
	
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
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
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
	int frames = 0;
	while (count <= 10)
	{
		ret = av_read_frame(pFormatCtx, packet);
		if (ret < 0)
		{
			count++;
			av_seek_frame(pFormatCtx, -1, 0 * AV_TIME_BASE, AVSEEK_FLAG_ANY);
			frames = 0;
		}

		if (packet->stream_index == videoindex)
		{
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

			if (ret < 0)
			{
				printf("Decode Error.\n");
				return -1;
			}
		/*	if (pFrame->key_frame)
			{
				frames++;
			}*/
			if (got_picture)
			{
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);

				Mat show(pFrame->height, pFrame->width, CV_8UC3);
				memcpy(show.data, out_buffer, size);

				imshow("gif", show);
				waitKey(50);
				printf("Succeed to decode 1 frame!\n");
				//frames++;
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
	return 0;
}