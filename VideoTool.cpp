#include <sstream>
#include <string>
#include <iostream>
//#include <opencv2\highgui.h>
#include "opencv2/highgui/highgui.hpp"
//#include <opencv2\cv.h>
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#define PORT 20232
#define DEL 500
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;


void delay(int milli_seconds)
{int micro_seconds = 1000 * milli_seconds;
 usleep(micro_seconds);
}

int setsock(int port,const char ip[100])
{    char hello[100];
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0)
    { printf("\nInvalid address/ Address not supported \n");
        return -1;}
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("%d %s\n",port,ip);
    return 0;
}

void mov(const char mv[100],int del)
{int i;
char hello[100];
    printf("%s",mv);
    for(i=0;i<strlen(mv);i++)
    {printf("%c\n",mv[i]);
      if(mv[i]=='f'||mv[i]=='b'||mv[i]=='r'||mv[i]=='l'||mv[i]=='s')
      {sprintf(hello,"%c\n",mv[i]);
       printf("%s sent\n",hello);
    send(sock , hello , strlen(hello) , 0 );
    delay(del);}
    }
   strcpy(hello,"s \n");
        send(sock , hello , strlen(hello) , 0 );
    printf("%s sent\n",hello);
    delay(del/2);
}

using namespace std;
using namespace cv;
//initial min and max HSV filter values.
//these will be changed using trackbars
struct robo{int x,y;}a,b;
int H_MIN = 164;
int H_MAX = 181;
int S_MIN = 49;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

int H_MIN1 =28;
int H_MAX1 = 56;
int S_MIN1 = 49;
int S_MAX1 = 256;
int V_MIN1 = 0;
int V_MAX1 = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const std::string windowName = "Original Image";
const std::string windowName1 = "HSV Image";
const std::string windowName2 = "Thresholded Image";
const std::string windowName3 = "After Morphological Operations";
const std::string windowName4 = "HSV Image2";
const std::string trackbarWindowName = "Trackbars";


void on_mouse(int e, int x, int y, int d, void *ptr)
{
	if (e == EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
}

void on_trackbar(int, void*)
{//This function gets called whenever a
 // trackbar position is changed
}

string intToString(int number) {


	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars() {
	//create window for trackbars


	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);


}
void drawObject(int x, int y, Mat &frame) {

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

	//UPDATE:JUNE 18TH, 2013
	//added 'if' and 'else' statements to prevent
	//memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25 > 0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25 < FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	if (x - 25 > 0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25 < FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);

	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 255, 0), 2);
	//cout << "x,y: " << x << ", " << y;

}
void morphOps(Mat &thresh) {

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);



}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed) {

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects < MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area > MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) {
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
				}
				else objectFound = false;


			}
			//let user know you found an object
			if (objectFound == true) {
				putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				//draw object location on screen
				//cout << x << "," << y;
				drawObject(x, y, cameraFeed);

			}


		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

int main(int argc, char* argv[])
{printf("Start");

	//some boolean variables for different functionality within this
	//program
	bool trackObjects = true;
	bool useMorphOps = true;

	Point p;
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	//matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold,threshold1;
	//x and y values for the location of the object
	a.x=0;
	a.y=0;
	b.x=0;
	b.y=0;
	//int x = 0, y = 0,x1=0,y1=0;
	//create slider bars for HSV filtering
	createTrackbars();
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open("rtmp://172.16.254.99/live/nimic");
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop

 int i,var=20;
 float m;
 int nd=0,od=0;
 printf("Socket begin");
 struct robo oldpos,newpos;
 setsock(20232,"193.226.12.217");
 strateg("ss",200);
bool old=true;
    while (1)
  {       //store image to matrix
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
		//filter HSV image between values and store filtered image to
		//threshold matrix
		//setObj(Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold,cameraFeed,HSV,useMorphOps,trackObjects,&a);
		//setObj(Scalar(H_MIN1,S_MIN1,V_MIN1),Scalar(H_MAX1,S_MAX1,V_MAX1),threshold1,cameraFeed,HSV,useMorphOps,trackObjects,&b);
		inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);
		inRange(HSV, Scalar(H_MIN1, S_MIN1, V_MIN1), Scalar(H_MAX1, S_MAX1, V_MAX1), threshold1);
		//perform morphological operations on thresholded image to eliminate noise
		//and emphasize the filtered object(s)
		if (useMorphOps)
		{morphOps(threshold);
		 morphOps(threshold1);}
		//pass in thresholded frame to our object tracking function
		//this function will return the x and y coordinates of the
		//filtered object
		if (trackObjects)
		{trackFilteredObject(a.x, a.y, threshold, cameraFeed);
         trackFilteredObject(b.x, b.y, threshold1,cameraFeed);}
		//show frames
		//imshow(windowName4, threshold1);
		//imshow(windowName2, threshold);
		imshow(windowName, cameraFeed);
		//imshow(windowName1, HSV);
		setMouseCallback("Original Image", on_mouse, &p);
		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(50);
	 //a-roz  b-galben
	if(old)
	{oldpos=b;
	 mov("fs",300);
	 printf("oldpos %d-%d\n",oldpos.x,oldpos.y);
	 old=false;
	}
	else
	{old = true;
     newpos=b;
	  printf("newpos %d-%d",newpos.x,newpos.y);
	  m=(float)(newpos.y-oldpos.y)/(float)(newpos.x-oldpos.x);
	  printf("Panta dreapta : %f \n",m);
	  if(oldpos.x!=newpos.x||oldpos.y!=newpos.y)
      {nd=sqrt((newpos.x-a.x)^2+(newpos.y-a.y)^2);
       od=sqrt((oldpos.x-a.x)^2+(oldpos.y-a.y)^2);
	printf("New dist= %d Old dist= %d \n",nd,od);
       if(nd>od)
	{printf("Reverse\n");
	  if(a.y>m*(a.x-newpos.x) +newpos.y+var)
	  {if(newpos.x<oldpos.x)
	    mov("rr",300);
    else mov("ll",300);}
	  else if(a.y<=m*(a.x-newpos.x) +newpos.y-var)
	  {if(newpos.x<oldpos.x)
	    mov("ll",300);
    else mov("rr",300);}
	}
	else{
	if(a.y>m*(a.x-newpos.x) +newpos.y+var) //mai mare sau mai mic y decat y dreapta
	{if(newpos.x<oldpos.x) //vad dak o ia la stanga sau la dreapta relativ la fereastra
	    mov("rf",300);
    else mov("lf",300);}
	  else if(a.y<m*(a.x-newpos.x) +newpos.y-var)
	 {if(newpos.x<oldpos.x)
	    mov("lf",300);
    else mov("rf",300);}
	 else {mov("ff",1000);}
	}
	}}}

	return 0;
}
