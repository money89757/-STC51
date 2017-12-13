#include <vector>
#include "wiringPi.h"
#include "dltype.h"
#include "net.h"
#include <string>
#include <opencv2/opencv.hpp>
#include "SocketMatTransmissionClient.h" 
using namespace cv;
using namespace std;
static int max_wid = 320;
#include <sys/time.h>
struct timeval tpstart,tpend;
unsigned long timeuses;
void timeRec()
{
    gettimeofday(&tpstart,0);
}
int timeRep()
{
    gettimeofday(&tpend,0);
    timeuses = (tpend.tv_sec-tpstart.tv_sec)*1000000+tpend.tv_usec-tpstart.tv_usec;
    timeuses /= 1;
    printf("use time: %uus\n",timeuses);
    return timeuses;
}

Mat rectan(Mat  img_src,Mat  mat_resize,float scale,qdetector* pdetector)
{
	vector<ObjRect> rects;
	Mat  img_face;
	Mat  img_face1;
	//timeRec();

	rects = pdetector->detectObjs(mat_resize.data, mat_resize.cols, mat_resize.rows, 10.8487, 100);
//printf("222\n");
       // timeRep();
	if (scale < 1) {
		for (int i = 0; i < rects.size(); ++i) {
			rects[i].left = rects[i].left / scale;
			rects[i].top = rects[i].top / scale;
			rects[i].right = rects[i].right / scale;
			rects[i].bottom = rects[i].bottom / scale;
		}
	}
        int face_nums = rects.size();
	Rect rect_face[face_nums];
        for(int i = 0; i < face_nums; i++)
	{
          rect_face[i].x = rects[i].left;
	  rect_face[i].y = rects[i].top;
	  rect_face[i].width = rects[i].right - rects[i].left;
	  rect_face[i].height = rects[i].bottom - rects[i].top;
          //rect_face[i].y = rects[i].left;
	  //rect_face[i].x = rects[i].top;
	  //rect_face[i].height = rects[i].right - rects[i].left;
	  //rect_face[i].width = rects[i].bottom - rects[i].top;

	  rectangle(img_src, rect_face[i], Scalar(0, 255, 0));
	//  printf("333\n");//
	  if(rect_face[i].x>0 && rect_face[i].y>0 && rect_face[i].x + rect_face[i].width < mat_resize.cols && rect_face[i].height + rect_face[i].y < mat_resize.rows){
	  	img_face = img_src(Rect(rect_face[i].x,rect_face[i].y,rect_face[i].width,rect_face[i].height));
	  }
//printf("444\n");
// &&rect_face[i].x+rect_face[i].width<mat_resize.cols
	  imwrite("face.jpg",img_face);

	 return img_face;
	}
return img_face1;
}

int main()
{
	FILE* fp = fopen("lightenedCNN_B/LightenedCNN_B.dat","rb");
	Mat img1 = imread("lightenedCNN_B/Abel_Pacheco_0001.png");
	Mat img2 = imread("lightenedCNN_B/Abel_Pacheco_0001.jpg");
	wiringPiSetup();
	pinMode(22,INPUT);

	 

	int i;
	Mat  img_gray;
	Mat  img_face;
	Mat  img_rects;
	Mat  mat_resize;
	
	//CnnNet myNet;
	//readAllParamFromdatFile(myNet, fp);
        timeRec();
        //float score = 0.0;

	qdetector* pdetector = NULL;
	char strModelPath[512] = { 0 };
	char* modelPath = "./model/";
	pdetector = qdetector::createInstance(modelPath);
	
	IplImage* frame;
	CvCapture *capture   = NULL;
	capture = cvCreateCameraCapture(0);
 	if (!capture)
        {
                 printf("camera is null\n");
        } 
	while(1){
		 
		int vol = digitalRead(22); 
	
		timeRec();
		frame = cvQueryFrame(capture);
		Mat img_src(frame,0);

		if(vol == 1){

			printf("someone here\n");
	
			//cvReleaseImage(&frame);
                        //transpose(img_src, img_src);
			cvtColor(img_src, img_gray, CV_BGR2GRAY);
			transpose(img_gray, img_gray);
			float scale = 1;
			resize(img_gray, mat_resize, Size(img_gray.cols*scale, img_gray.rows*scale));
			//if(img_face.empty())
			//{
				img_face =rectan(img_src,mat_resize,scale,pdetector);
				imwrite("1.jpg",img_src);
				
			//}
			if(img_face.empty()){printf("未检测到人脸\n");continue;}

			resize(img_face, img_face, cv::Size(128, 128), (0, 0), (0, 0), cv::INTER_LINEAR);
			SocketMatTransmissionClient socketMat;  
			if (socketMat.socketConnect("192.168.0.168", 6666) < 0)  
			{  
	      		      return 0;  
	     		}
			socketMat.transmit(img_face); 
			system("./led");
			socketMat.socketDisconnect();
			//printf("111\n");
			//score = computeSimilarScore(myNet, img1, img_face);
		}
	//socketMat.socketDisconnect();

		timeRep();
		//cout << "score: " << score << endl;
	}	
	cvReleaseImage(&frame);
	fclose(fp);
        //imwrite("1.jpg",img_src);
	return 0;
}

