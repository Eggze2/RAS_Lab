#include <opencv2/opencv.hpp>
#include<iostream>
#include <iostream>
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>  // 将ROS下的sensor_msgs/Image消息类型转化为cv::Mat数据类型
#include <sensor_msgs/image_encodings.h> // ROS下对图像进行处理
#include <image_transport/image_transport.h> // 用来发布和订阅图像信息

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "cvfinder/msg_card.h"
#include "std_msgs/Header.h"

using namespace cv;
using namespace std;
double area_judge(Mat3b frame){
    	vector<vector<Point>> contours;
	    vector<Vec4i> hierarchy;
        Mat  hsvImage, mask, outputImage;
        outputImage = Scalar(0, 0, 0);
	
        Mat outputImage1 ;	//最小外接正矩形画布	
		cvtColor(frame, hsvImage, COLOR_BGR2HSV);
        // 原代码
		//Define the range of "red" color in HSV colorspac
		// Scalar lowerLimit = Scalar(60, 100, 100);
		// Scalar upperLimit = Scalar(180, 255, 255);
		// //Threshold the HSV image to get only blue color
		// inRange(hsvImage, lowerLimit, upperLimit, mask);
		//Compute bitwise-ADD of input image and mask

        // 设定红色的HSV范围
        cv::Scalar lower_red1(0, 100, 100);
        cv::Scalar upper_red1(10, 255, 255);
        cv::Scalar lower_red2(160, 100, 100);
        cv::Scalar upper_red2(179, 255, 255);
        // 创建掩码
        cv::Mat mask1, mask2;
        cv::inRange(hsvImage, lower_red1, upper_red1, mask1);
        cv::inRange(hsvImage, lower_red2, upper_red2, mask2);
        mask = mask1 + mask2;
		bitwise_and(frame, frame, outputImage, mask = mask);
		//Run median filter on the output to smoothen it
		medianBlur(outputImage, outputImage, 5);

		cvtColor(outputImage,outputImage1,COLOR_BGR2GRAY);
		threshold(outputImage1,outputImage1, 60, 255, THRESH_BINARY);
		findContours(outputImage1, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());//寻找最外层轮廓
		Mat imageContours = Mat::zeros(outputImage.size(), CV_8UC1);	//最小外接正矩形画布
		vector<double> g_dConArea(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			g_dConArea[i] = contourArea(contours[i]);
		}
		//寻找面积最大的部分
		int max = 0;
		for (int i = 1; i < contours.size(); i++) {
			if (g_dConArea[i] > g_dConArea[max]) {
				max = i;
			}
		}

		if(contours.size()!=0){
		    // cout << "image max=" << g_dConArea[max] << endl;
		    drawContours(frame, contours, max, Scalar(255,255,255), 1, 8, hierarchy);
            waitKey(1);
            // cout<<"area ass"<<g_dConArea[max]<<endl;
            return g_dConArea[max];
		}
        return 0;
		// cout << "idx=" << max << endl;
		//绘制轮廓
		// imshow("out1",imageContours0);
}
 
int main(int argc, char* argv[])
{
    ros::init(argc, argv, "image_node");  // ros初始化，定义节点名为imageGet_node
    ros::NodeHandle nh;                      // 定义ros句柄
    image_transport::ImageTransport it(nh);  //  类似ROS句柄
    ros::Publisher flag_pub = nh.advertise<cvfinder::msg_card>("/flag_pub",1);
    ros::Time now = ros::Time::now();
    cvfinder::msg_card f;
    // initialize and allocate memory to load the video stream from camera
    VideoCapture camera0(4);
    camera0.set(CAP_PROP_FRAME_WIDTH,320);
    camera0.set(CAP_PROP_FRAME_HEIGHT,240);
    cout<<3<<endl;
    VideoCapture camera1(6);
    camera1.set(CAP_PROP_FRAME_WIDTH,320);
    camera1.set(CAP_PROP_FRAME_HEIGHT,240);
    cout<<4<<endl;
    
    // if( !camera0.isOpened() ) return 1;
    // if( !camera1.isOpened() ) return 1;
    cout<<1<<endl;
	Mat hsvImage, mask, outputImage;
	char ch;
 
    while(true && ros::ok()) {
        
        //grab and retrieve each frames of the video sequentially
        Mat3b frame0;
        camera0 >> frame0;

        Mat3b frame0_1;
        camera0 >> frame0_1;

        Mat3b frame1;
        camera1 >> frame1;
        // 这里是1，我改了下



        // imshow("Video0", frame0);
        // imshow("Video1", frame1);

        double area1=area_judge(frame0);
        // cout<<area_judge(frame0)<<std::endl;
        imshow("useless", frame0);
        double area2=area_judge(frame1);
        // cout<<area_judge(frame1)<<std::endl;
        imshow("out_Video1", frame1);
        // 然后用frame0_1 作图，专门用来检测红色范围区域
        cv::Mat hsv;
        cv::cvtColor(frame0_1, hsv, cv::COLOR_BGR2HSV);
        // 设定红色的HSV范围
        cv::Scalar lower_red1(0, 100, 100);
        cv::Scalar upper_red1(10, 255, 255);
        cv::Scalar lower_red2(160, 100, 100);
        cv::Scalar upper_red2(179, 255, 255);
        // 创建掩码
        cv::Mat mask1, mask2;
        cv::inRange(hsv, lower_red1, upper_red1, mask1);
        cv::inRange(hsv, lower_red2, upper_red2, mask2);
        cv::Mat mask = mask1 + mask2;
        // 寻找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                double max_area = 0;
        cv::Rect max_bbox;

        for (const auto& contour : contours) {
            // 获取包围目标的矩形
            cv::Rect boundingBox = cv::boundingRect(contour);

            // 计算面积大小
            double area = boundingBox.width * boundingBox.height;

            if (area > max_area) {
                // 更新最大面积和对应的矩形
                max_area = area;
                max_bbox = boundingBox;
            }
        }
        int c_x;
        
        if (max_area > 0) {
            // 获取中心点坐标
            cv::Point center(max_bbox.x + max_bbox.width / 2, max_bbox.y + max_bbox.height / 2);

            // 在原图上绘制矩形
            cv::rectangle(frame0_1, max_bbox, cv::Scalar(0, 255, 0), 2);

            // 在终端输出中心点坐标和面积大小
            std::cout << "Center coordinates: (" << center.x << ", " << center.y << "), Area: " << max_area << std::endl;
            c_x=center.x;
        }
        cout<<"area2    "<<area2<<endl;
        cv::imshow("video0", frame0_1);


        f.header.seq+=1;
        f.header.stamp=ros::Time::now();
        // if(area1<=1840 && area2<=1000){
        //     f.flag=0;
        // }else if((area1>1840 && area2<=1000)){
        //     f.flag=1;}

        // else if(area1>1840 && area2>1000){
        //     f.flag=3;
        // }
        if(max_area<=20 && area2<=2200){
            f.flag=0;
        }else if((max_area>20 && frame0_1.cols/2-40>c_x && area2<=2200)){
            // 顺时针
            f.flag=1;
        }
        else if(max_area>20 && c_x>frame0_1.cols/2+40 && area2<=2200){
            // 逆
            f.flag=2;
        }
        else if(max_area>20 && frame0_1.cols/2-40<c_x<frame0_1.cols/2+40 && area2<=2200){
            // 直走
            f.flag=3;
        }
        else if(20<=max_area<=800 && area2<=2200){
            f.flag=3;
        }
        else if(max_area>800 && area2>2200){
            f.flag=4;
        }
        flag_pub.publish(f);


        int c = waitKey(1);
        if(27 == char(c)) {
            break;
        }
 
    }
 
 
    return 0;
 
}

// 备用代码2
// #include <opencv2/opencv.hpp>
// #include<iostream>
// #include <iostream>
// #include <ros/ros.h>
// #include <cv_bridge/cv_bridge.h>  // 将ROS下的sensor_msgs/Image消息类型转化为cv::Mat数据类型
// #include <sensor_msgs/image_encodings.h> // ROS下对图像进行处理
// #include <image_transport/image_transport.h> // 用来发布和订阅图像信息

// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/videoio.hpp>
// #include "cvfinder/msg_card.h"
// #include "std_msgs/Header.h"

// using namespace cv;
// using namespace std;
// double area_judge(Mat3b frame){
//     	vector<vector<Point>> contours;
// 	    vector<Vec4i> hierarchy;
//         Mat  hsvImage, mask, outputImage;
//         outputImage = Scalar(0, 0, 0);
	
//         Mat outputImage1 ;	//最小外接正矩形画布	
// 		cvtColor(frame, hsvImage, COLOR_BGR2HSV);
// 		//Define the range of "blue" color in HSV colorspace
// 		Scalar lowerLimit = Scalar(60, 100, 100);
// 		Scalar upperLimit = Scalar(180, 255, 255);
// 		//Threshold the HSV image to get only blue color
// 		inRange(hsvImage, lowerLimit, upperLimit, mask);
// 		//Compute bitwise-ADD of input image and mask
// 		bitwise_and(frame, frame, outputImage, mask = mask);
// 		//Run median filter on the output to smoothen it
// 		medianBlur(outputImage, outputImage, 5);

// 		cvtColor(outputImage,outputImage1,COLOR_BGR2GRAY);
// 		threshold(outputImage1,outputImage1, 60, 255, THRESH_BINARY);
// 		findContours(outputImage1, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());//寻找最外层轮廓
// 		Mat imageContours = Mat::zeros(outputImage.size(), CV_8UC1);	//最小外接正矩形画布
// 		vector<double> g_dConArea(contours.size());
// 		for (int i = 0; i < contours.size(); i++)
// 		{
// 			g_dConArea[i] = contourArea(contours[i]);
// 		}
// 		//寻找面积最大的部分
// 		int max = 0;
// 		for (int i = 1; i < contours.size(); i++) {
// 			if (g_dConArea[i] > g_dConArea[max]) {
// 				max = i;
// 			}
// 		}

// 		if(contours.size()!=0){
// 		    // cout << "image max=" << g_dConArea[max] << endl;
// 		    drawContours(frame, contours, max, Scalar(255,255,255), 1, 8, hierarchy);
//             waitKey(1);
//             return g_dConArea[max];
// 		}
//         return 0;
// 		// cout << "idx=" << max << endl;
// 		//绘制轮廓
// 		// imshow("out1",imageContours0);
// }
 
// int main(int argc, char* argv[])
// {
//     ros::init(argc, argv, "image_node");  // ros初始化，定义节点名为imageGet_node
//     ros::NodeHandle nh;                      // 定义ros句柄
//     image_transport::ImageTransport it(nh);  //  类似ROS句柄
//     ros::Publisher flag_pub = nh.advertise<cvfinder::msg_card>("/flag_pub",1);
//     ros::Time now = ros::Time::now();
//     cvfinder::msg_card f;
//     // initialize and allocate memory to load the video stream from camera
//     VideoCapture camera0(6);
//     camera0.set(CAP_PROP_FRAME_WIDTH,320);
//     camera0.set(CAP_PROP_FRAME_HEIGHT,240);
//     cout<<3<<endl;
//     VideoCapture camera1(4);
//     camera1.set(CAP_PROP_FRAME_WIDTH,320);
//     camera1.set(CAP_PROP_FRAME_HEIGHT,240);
//     cout<<4<<endl;


//     if( !camera0.isOpened() ) return 1;
//     if( !camera1.isOpened() ) return 1;
//     cout<<1<<endl;
// 	Mat hsvImage, mask, outputImage;
// 	char ch;
 
//     while(true && ros::ok()) {
        
//         //grab and retrieve each frames of the video sequentially
//         Mat3b frame0;
//         camera0 >> frame0;
 
//         Mat3b frame1;
//         camera1 >> frame1;
 

 // #include <opencv2/opencv.hpp>
// #include<iostream>
// #include <iostream>
// #include <ros/ros.h>
// #include <cv_bridge/cv_bridge.h>  // 将ROS下的sensor_msgs/Image消息类型转化为cv::Mat数据类型
// #include <sensor_msgs/image_encodings.h> // ROS下对图像进行处理
// #include <image_transport/image_transport.h> // 用来发布和订阅图像信息

// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/videoio.hpp>
// #include "cvfinder/msg_card.h"
// #include "std_msgs/Header.h"

// using namespace cv;
// using namespace std;
// double area_judge(Mat3b frame){
//     	vector<vector<Point>> contours;
// 	    vector<Vec4i> hierarchy;
//         Mat  hsvImage, mask, outputImage;
//         outputImage = Scalar(0, 0, 0);
	
//         Mat outputImage1 ;	//最小外接正矩形画布	
// 		cvtColor(frame, hsvImage, COLOR_BGR2HSV);
//         // 原代码
// 		//Define the range of "red" color in HSV colorspac
// 		// Scalar lowerLimit = Scalar(60, 100, 100);
// 		// Scalar upperLimit = Scalar(180, 255, 255);
// 		// //Threshold the HSV image to get only blue color
// 		// inRange(hsvImage, lowerLimit, upperLimit, mask);
// 		//Compute bitwise-ADD of input image and mask

//         // 设定红色的HSV范围
//         cv::Scalar lower_red1(0, 100, 100);
//         cv::Scalar upper_red1(10, 255, 255);
//         cv::Scalar lower_red2(160, 100, 100);
//         cv::Scalar upper_red2(179, 255, 255);
//         // 创建掩码
//         cv::Mat mask1, mask2;
//         cv::inRange(hsvImage, lower_red1, upper_red1, mask1);
//         cv::inRange(hsvImage, lower_red2, upper_red2, mask2);
//         mask = mask1 + mask2;
// 		bitwise_and(frame, frame, outputImage, mask = mask);
// 		//Run median filter on the output to smoothen it
// 		medianBlur(outputImage, outputImage, 5);

// 		cvtColor(outputImage,outputImage1,COLOR_BGR2GRAY);
// 		threshold(outputImage1,outputImage1, 60, 255, THRESH_BINARY);
// 		findContours(outputImage1, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());//寻找最外层轮廓
// 		Mat imageContours = Mat::zeros(outputImage.size(), CV_8UC1);	//最小外接正矩形画布
// 		vector<double> g_dConArea(contours.size());
// 		for (int i = 0; i < contours.size(); i++)
// 		{
// 			g_dConArea[i] = contourArea(contours[i]);
// 		}
// 		//寻找面积最大的部分
// 		int max = 0;
// 		for (int i = 1; i < contours.size(); i++) {
// 			if (g_dConArea[i] > g_dConArea[max]) {
// 				max = i;
// 			}
// 		}

// 		if(contours.size()!=0){
// 		    // #include <opencv2/opencv.hpp>
// #include<iostream>
// #include <iostream>
// #include <ros/ros.h>
// #include <cv_bridge/cv_bridge.h>  // 将ROS下的sensor_msgs/Image消息类型转化为cv::Mat数据类型
// #include <sensor_msgs/image_encodings.h> // ROS下对图像进行处理
// #include <image_transport/image_transport.h> // 用来发布和订阅图像信息

// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/videoio.hpp>
// #include "cvfinder/msg_card.h"
// #include "std_msgs/Header.h"

// using namespace cv;
// using namespace std;
// double area_judge(Mat3b frame){
//     	vector<vector<Point>> contours;
// 	    vector<Vec4i> hierarchy;
//         Mat  hsvImage, mask, outputImage;
//         outputImage = Scalar(0, 0, 0);
	
//         Mat outputImage1 ;	//最小外接正矩形画布	
// 		cvtColor(frame, hsvImage, COLOR_BGR2HSV);
//         // 原代码
// 		//Define the range of "red" color in HSV colorspac
// 		// Scalar lowerLimit = Scalar(60, 100, 100);
// 		// Scalar upperLimit = Scalar(180, 255, 255);
// 		// //Threshold the HSV image to get only blue color
// 		// inRange(hsvImage, lowerLimit, upperLimit, mask);
// 		//Compute bitwise-ADD of input image and mask

//         // 设定红色的HSV范围
//         cv::Scalar lower_red1(0, 100, 100);
//         cv::Scalar upper_red1(10, 255, 255);
//         cv::Scalar lower_red2(160, 100, 100);
//         cv::Scalar upper_red2(179, 255, 255);
//         // 创建掩码
//         cv::Mat mask1, mask2;
//         cv::inRange(hsvImage, lower_red1, upper_red1, mask1);
//         cv::inRange(hsvImage, lower_red2, upper_red2, mask2);
//         mask = mask1 + mask2;
// 		bitwise_and(frame, frame, outputImage, mask = mask);
// 		//Run median filter on the output to smoothen it
// 		medianBlur(outputImage, outputImage, 5);

// 		cvtColor(outputImage,outputImage1,COLOR_BGR2GRAY);
// 		threshold(outputImage1,outputImage1, 60, 255, THRESH_BINARY);
// 		findContours(outputImage1, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());//寻找最外层轮廓
// 		Mat imageContours = Mat::zeros(outputImage.size(), CV_8UC1);	//最小外接正矩形画布
// 		vector<double> g_dConArea(contours.size());
// 		for (int i = 0; i < contours.size(); i++)
// 		{
// 			g_dConArea[i] = contourArea(contours[i]);
// 		}
// 		//寻找面积最大的部分
// 		int max = 0;
// 		for (int i = 1; i < contours.size(); i++) {
// 			if (g_dConArea[i] > g_dConArea[max]) {
// 				max = i;
// 			}
// 		}

// 		if(contours.size()!=0){
// 		    // cout << "image max=" << g_dConArea[max] << endl;
// 		    drawContours(frame, contours, max, Scalar(255,255,255), 1, 8, hierarchy);
//             waitKey(1);
//             // cout<<"area ass"<<g_dConArea[max]<<endl;
//             return g_dConArea[max];
// 		}
//         return 0;
// 		// cout << "idx=" << max << endl;
// 		//绘制轮廓
// 		// imshow("out1",imageContours0);
// }
 
// int main(int argc, char* argv[])
// {
//     ros::init(argc, argv, "image_node");  // ros初始化，定义节点名为imageGet_node
//     ros::NodeHandle nh;                      // 定义ros句柄
//     image_transport::ImageTransport it(nh);  //  类似ROS句柄
//     ros::Publisher flag_pub = nh.advertise<cvfinder::msg_card>("/flag_pub",1);
//     ros::Time now = ros::Time::now();
//     cvfinder::msg_card f;
//     // initialize and allocate memory to load the video stream from camera
//     VideoCapture camera0(6);
//     camera0.set(CAP_PROP_FRAME_WIDTH,320);
//     camera0.set(CAP_PROP_FRAME_HEIGHT,240);
//     cout<<3<<endl;
//     VideoCapture camera1(4);
//     camera1.set(CAP_PROP_FRAME_WIDTH,320);
//     camera1.set(CAP_PROP_FRAME_HEIGHT,240);
//     cout<<4<<endl;
    
//     // if( !camera0.isOpened() ) return 1;
//     // if( !camera1.isOpened() ) return 1;
//     cout<<1<<endl;
// 	Mat hsvImage, mask, outputImage;
// 	char ch;
 
//     while(true && ros::ok()) {
        
//         //grab and retrieve each frames of the video sequentially
//         Mat3b frame0;
//         camera0 >> frame0;

//         Mat3b frame0_1;
//         camera0 >> frame0_1;

//         Mat3b frame1;
//         camera1 >> frame1;
//         // 这里是1，我改了下



//         // imshow("Video0", frame0);
//         // imshow("Video1", frame1);

//         double area1=area_judge(frame0);
//         // cout<<area_judge(frame0)<<std::endl;
//         imshow("useless", frame0);
//         double area2=area_judge(frame1);
//         // cout<<area_judge(frame1)<<std::endl;
//         imshow("out_Video1", frame1);
//         // 然后用frame0_1 作图，专门用来检测红色范围区域
//         cv::Mat hsv;
//         cv::cvtColor(frame0_1, hsv, cv::COLOR_BGR2HSV);
//         // 设定红色的HSV范围
//         cv::Scalar lower_red1(0, 100, 100);
//         cv::Scalar upper_red1(10, 255, 255);
//         cv::Scalar lower_red2(160, 100, 100);
//         cv::Scalar upper_red2(179, 255, 255);
//         // 创建掩码
//         cv::Mat mask1, mask2;
//         cv::inRange(hsv, lower_red1, upper_red1, mask1);
//         cv::inRange(hsv, l
//         imshow("Video0", frame0);
//         imshow("Video1", frame1);

//         double area1=area_judge(frame0);
//         // cout<<area_judge(frame0)<<std::endl;
//         imshow("out_Video0", frame0);
//         double area2=area_judge(frame1);
//         // cout<<area_judge(frame1)<<std::endl;
//         imshow("out_Video1", frame1);
//         f.header.seq+=1;
//         f.header.stamp=ros::Time::now();
//         if(area1<=3840 && area2<=1000){
//             f.flag=0;
//         }else if((area1>=3840 && area2<=1000)){
//             f.flag=1;
//         }else if(area1>=8840 && area2>1000){
//             f.flag=3;
//         }
        
//         flag_pub.publish(f);


//         int c = waitKey(1);
//         if(27 == char(c)) {
//             break;
//         }
 
//     }
 
 
//     return 0;
 
// }


// 备用代码
// #include <iostream>
// #include <ros/ros.h>
// #include <cv_bridge/cv_bridge.h>  // 将ROS下的sensor_msgs/Image消息类型转化为cv::Mat数据类型
// #include <sensor_msgs/image_encodings.h> // ROS下对图像进行处理
// #include <image_transport/image_transport.h> // 用来发布和订阅图像信息

// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/videoio.hpp>
// #include <opencv2/core/utility.hpp>
// #include "opencv2/video/tracking.hpp"
// #include "opencv2/imgproc.hpp"
// #include "opencv2/videoio.hpp"
// #include "opencv2/highgui.hpp"
 
// #include <iostream>
// #include <ctype.h>
// using namespace cv;
// using namespace std;

// Mat image;

// bool backprojMode = false;
// bool selectObject = false;
// int trackObject = 0;
// bool showHist = true;
// Point origin;
// Rect selection;
// int vmin = 10, vmax = 256, smin = 30;

// // User draws box around object to track. This triggers CAMShift to start tracking
// static void onMouse(int event, int x, int y, int, void*)     //鼠标标记目标物体1
// {
//     if (selectObject)
//     {
//         selection.x = MIN(x, origin.x);                         //标记框选区域的X坐标
//         selection.y = MIN(y, origin.y);                         //标记框选区域的y坐标
//         selection.width = std::abs(x - origin.x);               //框选区域的宽度
//         selection.height = std::abs(y - origin.y);              //框选区域的高度

//         selection &= Rect(0, 0, image.cols, image.rows);
//     }
//     cout << selection.width <<"啊哈哈哈哈" << endl;
    
//     switch (event){
// case EVENT_LBUTTONDOWN:
// origin = Point(x, y);
// selection = Rect(x, y, 0, 0);
// selectObject = true;
// break;
// case EVENT_LBUTTONUP:
// selectObject = false;
// if (selection.width > 0 && selection.height > 0)
// trackObject = -1; // Set up CAMShift properties in main() loop
// break;
// }
// }

// string hot_keys =
// "\n\nHot keys: \n"
// "\tESC - quit the program\n"
// "\tc - stop the tracking\n"
// "\tb - switch to/from backprojection view\n"
// "\th - show/hide object histogram\n"
// "\tp - pause video\n"
// "To initialize tracking, select the object with mouse\n";

// static void help(const char** argv)
// {
// cout << "\nThis is a demo that shows mean-shift based tracking\n"
// "You select a color objects such as your face and it tracks it.\n"
// "This reads from video camera (0 by default, or the camera number the user enters\n"
// "Usage: \n\t";
// cout << argv[0] << " [camera number]\n";
// cout << hot_keys;
// }

// const char* keys =
// {
// "{help h | | show help message}{@camera_number| 0 | camera number}"
// };

// int main(int argc, const char** argv)
// {

//     VideoCapture cap;
//     Rect trackWindow;
//     int hsize = 16;
//     float hranges[] = { 0,180 };
//     const float* phranges = hranges;
//     CommandLineParser parser(argc, argv, keys);
//     if (parser.has("help"))
//     {
//         help(argv);
//         return 0;
//     }
//     int camNum = 6;
//     cap.open(camNum);

//     if (!cap.isOpened())
//     {
//     help(argv);
//     cout << "***Could not initialize capturing...***\n";
//     cout << "Current parameter's value: \n";
//     parser.printMessage();
//     return -1;
//     }
//     cout << hot_keys;
//     namedWindow("Histogram", 0);
//     namedWindow("CamShift Demo", 0);
//     setMouseCallback("CamShift Demo", onMouse, 0);
//     createTrackbar("Vmin", "CamShift Demo", &vmin, 256, 0);
//     createTrackbar("Vmax", "CamShift Demo", &vmax, 256, 0);
//     createTrackbar("Smin", "CamShift Demo", &smin, 256, 0);

//     Mat frame, hsv, hue, mask, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
// bool paused = false;
// for (;;)
//     {
//         if (!paused)
//         {
//             cap >> frame;
//             if (frame.empty())
//                 break;
//         }

//         frame.copyTo(image);

//         if (!paused)
//         {
//             cvtColor(image, hsv, COLOR_BGR2HSV);

//             if (trackObject)
//             {
//                 int _vmin = vmin, _vmax = vmax;

//                 inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
//                     Scalar(180, 256, MAX(_vmin, _vmax)), mask);
//                 int ch[] = { 0, 0 };
//                 hue.create(hsv.size(), hsv.depth());
//                 mixChannels(&hsv, 1, &hue, 1, ch, 1);

//                 if (trackObject < 0)
//                 {
//                     // Object has been selected by user, set up CAMShift search properties once
//                     Mat roi(hue, selection), maskroi(mask, selection);
//                     calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
//                     normalize(hist, hist, 0, 255, NORM_MINMAX);

//                     trackWindow = selection;
//                     trackObject = 1; // Don't set up again, unless user selects new ROI

//                     histimg = Scalar::all(0);
//                     int binW = histimg.cols / hsize;
//                     Mat buf(1, hsize, CV_8UC3);
//                     for (int i = 0; i < hsize; i++)
//                         buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i * 180. / hsize), 255, 255);
//                     cvtColor(buf, buf, COLOR_HSV2BGR);

//                     for (int i = 0; i < hsize; i++)
//                     {
//                         int val = saturate_cast<int>(hist.at<float>(i) * histimg.rows / 255);
//                         rectangle(histimg, Point(i * binW, histimg.rows),
//                             Point((i + 1) * binW, histimg.rows - val),
//                             Scalar(buf.at<Vec3b>(i)), -1, 8);
//                     }
//                 }

//                 // Perform CAMShift
//                 calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
//                 backproj &= mask;
//                 RotatedRect trackBox = CamShift(backproj, trackWindow,
//                     TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
//                 if (trackWindow.area() <= 1)
//                 {
//                     int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
//                     trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
//                         trackWindow.x + r, trackWindow.y + r) &
//                         Rect(0, 0, cols, rows);
//                 }

//                 if (backprojMode)
//                     cvtColor(backproj, image, COLOR_GRAY2BGR);
//                 ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
//             }
//         }
//         else if (trackObject < 0)
//             paused = false;

//         if (selectObject && selection.width > 0 && selection.height > 0)
//         {
//             Mat roi(image, selection);
//             bitwise_not(roi, roi);
//         }

//         imshow("CamShift Demo", image);
//         imshow("Histogram", histimg);

//         char c = (char)waitKey(10);
//         if (c == 27)
//             break;
//         switch (c)
//         {
//         case 'b':
//             backprojMode = !backprojMode;
//             break;
//         case 'c':
//             trackObject = 0;
//             histimg = Scalar::all(0);
//             break;
//         case 'h':
//             showHist = !showHist;
//             if (!showHist)
//                 destroyWindow("Histogram");
//             else
//                 namedWindow("Histogram", 1);
//             break;
//         case 'p':
//             paused = !paused;
//             break;
//         default:
//             ;
//         }
//     }
//     return 0;
// }
