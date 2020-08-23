#include "image_encoder.h"
#include <fstream>

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif  // _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif  // _DEBUG

void ImageEncoder::encoder2vetor(const vector<string> &img_path, vector<vector<int>> &img_codes)
{
	for (int i = 0; i != img_path.size();++i)
	{
		string tmp = img_path[i];
		Mat img=imread(tmp);
		if (!img.data)
		{
			ProGif pg(1);
			vector<Mat> gif_frames;
			pg.pickGif2Mats(tmp.c_str(), 1, gif_frames);
			if(!gif_frames.empty()) img = gif_frames[0];
		}
		if (!img.data)
		{
			cout << "can not read image" << endl;
			continue;
		}

		Mat img_pro;
		img_prep.ImgResize(img, img_pro, obj_len);
		vector<int> code;
		img_prep.imgRGB_HSVQuantization(img_pro, code);
		img_codes.push_back(code);
	}


//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	/*int *c = new int[10];
	vector<int> a;
	a.push_back(1);*/
}