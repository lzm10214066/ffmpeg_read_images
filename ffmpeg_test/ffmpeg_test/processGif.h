#pragma once

#include <iostream>
#include "opencv2\opencv.hpp"

#include "gif_ffmpeg.h"

using namespace std;
using namespace cv;

class ProGif
{
	int max_num;
	double th;
	int timeStep;
	double simiTwoImages(Mat &imga, Mat &imgb);
	int  pickGif2Mats(const char* filename, int max_num, vector<Mat> &gif_frames);
	void duplicateFilter(vector<Mat> &frames, double th);
	int  gif2frames(const char* filename, const string &folderName);

public:
	ProGif(int _max_num=5,double _th=0.8,int _timeStep=10);
	int  showGif(const char* filename,int repeat);
	void proGif(string &fileName, vector<Mat> &frames);
	int pickGifImages(const char *filepath, const string &folder);
	void readImage(const char *filename, Mat &img);
};


