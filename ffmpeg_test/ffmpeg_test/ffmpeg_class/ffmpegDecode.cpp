#include "ffmpegDecode.h"

ffmpegDecode :: ~ffmpegDecode()
{
    pCvMat->release();
    //�ͷű��ζ�ȡ��֡�ڴ�
    av_free_packet(packet);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}

ffmpegDecode::ffmpegDecode(const char * file) :filepath(file)
{
    pAvFrame = NULL/**pFrameRGB = NULL*/;
    pFormatCtx  = NULL;
    pCodecCtx   = NULL;
    pCodec      = NULL;

    pCvMat = new cv::Mat();
    i=0;
    videoindex=0;

    ret = 0;
    got_picture = 0;
    img_convert_ctx = NULL;
    y_size = 0;
    packet = NULL;

    init();
    openDecode();
    prepare();

    return;
}

void ffmpegDecode :: init()
{
    //ffmpegע�Ḵ�������������ȵĺ���av_register_all()��
    //�ú��������л���ffmpeg��Ӧ�ó����м������ǵ�һ�������õġ�ֻ�е����˸ú���������ʹ�ø��������������ȡ�
    //����ע�������е��ļ���ʽ�ͱ�������Ŀ⣬�������ǽ����Զ���ʹ���ڱ��򿪵ĺ��ʸ�ʽ���ļ��ϡ�ע����ֻ��Ҫ���� av_register_all()һ�Σ����������������main()�����������������ϲ����Ҳ����ֻע���ض��ĸ�ʽ�ͱ������������ͨ����û�б�Ҫ��������
    av_register_all();

    //pFormatCtx = avformat_alloc_context();
    //����Ƶ�ļ�,ͨ������filepath������ļ��������������ȡ�ļ���ͷ�����Ұ���Ϣ���浽���Ǹ���AVFormatContext�ṹ���С�
    //���2����������ָ��������ļ���ʽ�������С�͸�ʽ���������������������Ϊ��NULL����0��libavformat���Զ������Щ������
    if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0)
    {
        printf("�޷����ļ�\n");
        return;
    }

    //�����ļ�������Ϣ,avformat_open_input����ֻ�Ǽ�����ļ���ͷ��������Ҫ������ļ��е�������Ϣ
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return;
	}
    return;
}

void ffmpegDecode :: openDecode()
{
    //�����ļ��ĸ��������ҵ���һ����Ƶ��������¼�����ı�����Ϣ
    videoindex = -1;
    for(i=0; i<pFormatCtx->nb_streams; i++) 
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    }
    if(videoindex==-1)
    {
        printf("Didn't find a video stream.\n");
        return;
    }
    pCodecCtx=pFormatCtx->streams[videoindex]->codec;

    //�ڿ��������֧�ָø�ʽ�Ľ�����
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
    {
        printf("Codec not found.\n");
        return;
    }

    //�򿪽�����
    if(avcodec_open2(pCodecCtx, pCodec,NULL) < 0)
    {
        printf("Could not open codec.\n");
        return;
    }
}

void ffmpegDecode :: prepare()
{
    //����һ��ָ֡�룬ָ�������ԭʼ֡
	pAvFrame = av_frame_alloc();
    y_size = pCodecCtx->width * pCodecCtx->height;
    //����֡�ڴ�
    packet=(AVPacket *)av_malloc(sizeof(AVPacket));
    av_new_packet(packet, y_size);

    //���һ����Ϣ-----------------------------
    printf("�ļ���Ϣ-----------------------------------------\n");
    av_dump_format(pFormatCtx,0,filepath,0);
    //av_dump_formatֻ�Ǹ����Ժ���������ļ���������Ƶ���Ļ�����Ϣ�ˣ�֡�ʡ��ֱ��ʡ���Ƶ�����ȵ�
    printf("-------------------------------------------------\n");
}

int ffmpegDecode :: readOneFrame()
{
    int result = 0;
    result = av_read_frame(pFormatCtx, packet);
    return result;
}

cv::Mat ffmpegDecode :: getDecodedFrame()
{
    if(packet->stream_index==videoindex)
    {
        //����һ��֡
        ret = avcodec_decode_video2(pCodecCtx, pAvFrame, &got_picture, packet);
        if(ret < 0)
        {
            printf("�������\n");
            return cv::Mat();
        }
        if(got_picture)
        {
            //���ݱ�����Ϣ������Ⱦ��ʽ
            if(img_convert_ctx == NULL){
                img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                    pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                    AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL); 
            }    
            //----------------------opencv
            if (pCvMat->empty())
            {
                pCvMat->create(cv::Size(pCodecCtx->width, pCodecCtx->height),CV_8UC3);
            }

            if(img_convert_ctx != NULL)  
            {  
                get(pCodecCtx, img_convert_ctx, pAvFrame);
            }
        }
    }
    av_free_packet(packet);

    return *pCvMat;
}

cv::Mat ffmpegDecode :: getLastFrame()
{
    ret = avcodec_decode_video2(pCodecCtx, pAvFrame, &got_picture, packet);
    if(got_picture) 
    {  
        //���ݱ�����Ϣ������Ⱦ��ʽ
		img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

        if(img_convert_ctx != NULL)  
        {  
            get(pCodecCtx, img_convert_ctx,pAvFrame);
        }  
    } 
    return *pCvMat;
}

void ffmpegDecode :: get(AVCodecContext    * pCodecCtx, SwsContext * img_convert_ctx, AVFrame * pFrame)
{
    if (pCvMat->empty())
    {
        pCvMat->create(cv::Size(pCodecCtx->width, pCodecCtx->height),CV_8UC3);
    }

    AVFrame    *pFrameRGB = NULL;
    uint8_t  *out_bufferRGB = NULL;
	pFrameRGB = av_frame_alloc();

    //��pFrameRGB֡���Ϸ�����ڴ�;
    int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
    out_bufferRGB = new uint8_t[size];
    avpicture_fill((AVPicture *)pFrameRGB, out_bufferRGB, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

    //YUV to RGB
    sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

    memcpy(pCvMat->data,out_bufferRGB,size);

    delete[] out_bufferRGB;
    av_free(pFrameRGB);
}