#include <opencv2/opencv.hpp>
#include <iostream>

#include "image_encoder.h"

#include "imageRead.h"


using namespace std;
using namespace cv;



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


int main(int argc, char* argv[])
{
	string imageFile = "path.txt";
	vector<string> image_path;
	int imageCount = readImageFile(imageFile, image_path);

	double t = (double)getTickCount();

	ImageEncoder img_encoder;

	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);
	{
		vector<vector<int>> img_codes;


		//img_encoder.encoder2vetor(image_path,img_codes);



		//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		

		//int *c = new int[10];

		img_encoder.encoder2vetor(image_path, img_codes);

		

		t = ((double)getTickCount() - t) / getTickFrequency();
		cout << "Times passed in ms: " << t * 1000 << endl;
	}
	_CrtMemCheckpoint(&s2);
	if (_CrtMemDifference(&s3, &s1, &s2))
		_CrtMemDumpStatistics(&s3);
	/*	string folderName = "res";
		for (int i = 0; i < gif_frames.size(); ++i)
		{
		char srcToSaved[500];
		string name = getFileName(s);
		sprintf(srcToSaved, "%s/%s_%d.png", folderName.c_str(), name.c_str(), i);
		string tt(srcToSaved);
		imwrite(srcToSaved, gif_frames[i]);
		}*/

	//_CrtDumpMemoryLeaks();
	return 0;
}


