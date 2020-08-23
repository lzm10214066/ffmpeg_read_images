#ifndef __FFMPEG_DECODE_H__
#define __FFMPEG_DECODE_H__

#include <iostream>
#include "opencv2\opencv.hpp"

#include "gif_ffmpeg.h"

using namespace std;
using namespace cv;


class ffmpegDecode
{
public:
    ffmpegDecode(const char * file = NULL);
    ~ffmpegDecode();

    cv::Mat getDecodedFrame();
    cv::Mat getLastFrame();
    int readOneFrame();
    int getFrameInterval();

private:
    AVFrame    *pAvFrame;
    AVFormatContext    *pFormatCtx;
    AVCodecContext    *pCodecCtx;
    AVCodec            *pCodec;

    int    i; 
    int videoindex;

    const char *filepath;
    int ret, got_picture;
    SwsContext *img_convert_ctx;
    int y_size;
    AVPacket *packet;

    cv::Mat *pCvMat;

    void init();
    void openDecode();
    void prepare();
    void get(AVCodecContext *pCodecCtx, SwsContext *img_convert_ctx,AVFrame    *pFrame);
};

#endif
