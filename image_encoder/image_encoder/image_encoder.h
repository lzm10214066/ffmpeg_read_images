#ifndef _IMAGE_ENCODER_H
#define _IMAGE_ENCODER_H

#include <iostream>
#include "opencv2/opencv.hpp"
#include "ImagePreProcess.h"
#include "processGif.h"

using namespace std;
using namespace cv;

class ImageEncoder
{
	ImagePreProcess img_prep;
	ProGif pro_gif;
	int obj_len;

	inline string getExten(string &filename) const
	{
		int n = filename.find_last_of('.');
		int m = filename.find_last_not_of(' ');
		return filename.substr(n + 1,m-n);
	}

public:
	ImageEncoder(int _obj_len=64) :obj_len(_obj_len){}

	void encoder2vetor(const vector<string> &img_path, vector<vector<int>> &img_codes);
};
#endif