#include <iostream>
#include "opencv2\opencv.hpp"
#include "processGif.h"
//#include "ffmpegDecode.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{

	string gif_path = "t1.mp4";
	double t = (double)getTickCount();

	/*ffmpegDecode ffD(gif_path.c_str());

	ffD.readOneFrame();
	Mat img=ffD.getDecodedFrame();
	
	int k = 10;
	while (k--){
		ffD.readOneFrame();
		ffD.getDecodedFrame();
	}
	img = ffD.getDecodedFrame();*/

	ProGif p;

	/*vector<Mat> gif_frames;
	p.proGif(gif_path, gif_frames);

	for (int i = 0; i<gif_frames.size(); ++i)
	{
		char srcToSaved[500];
		sprintf(srcToSaved, "%s/%s_%d.png", "res", "gif", i);
		Mat show = gif_frames[i];
		imwrite(srcToSaved, show);
	}
	imwrite("res.jpg", gif_frames[0]);
	p.showGif(gif_path.c_str(),1);*/
	p.pickGifImages(gif_path.c_str(), "res_1");
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Times passed in ms: " << t * 1000 << endl;
	return 0;
}


