#include <iostream>
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>  // 将ROS下的sensor_msgs/Image消息类型转化为cv::Mat数据类型
#include <sensor_msgs/image_encodings.h> // ROS下对图像进行处理
#include <image_transport/image_transport.h> // 用来发布和订阅图像信息

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
 
#include <iostream>
#include <ctype.h>
#include "cv_with_car/center_x.h"
#include<time.h>
#include "std_msgs/Header.h"
using namespace cv;
using namespace std;
Mat image;
 
bool backprojMode = false;
bool selectObject = false;
int trackObject = 0;
bool showHist = true;
Point origin;
Rect selection;
int vmin = 10, vmax = 256, smin = 30;
 
// User draws box around object to track. This triggers CAMShift to start tracking
static void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )
    {
        selection.x = MIN(x, origin.x);                     //标记框选区域的X坐标
        selection.y = MIN(y, origin.y);                     //标记框选区域的y坐标
        selection.width = std::abs(x - origin.x);           //框选区域的宽度
        selection.height = std::abs(y - origin.y);          //框选区域的高度
 
        selection &= Rect(0, 0, image.cols, image.rows);
    }

    // cout<<origin.x<<endl;
    // cout<<selection.width<<endl;

    switch( event )
    {
    case EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selection = Rect(x,y,0,0);
        selectObject = true;
        break;
    case EVENT_LBUTTONUP:
        selectObject = false;
        if( selection.width > 0 && selection.height > 0 )
            trackObject = -1;   // Set up CAMShift properties in main() loop
        break;
    }
}
 
static void showHelp()
{
	cout << "\n\n\tThis Demo shows mean-shift based tracking\n"
		"\tPlease use the mouse to select a colored object and track it\n"
		"----------------------------------------------------------------------------\n" ;
 
	cout << "\n\tOperating Instructions:\n"
		"\tESC - quit the program\n"
	    "\tc - stop the tracking\n"
	    "\tb - switch to/from backprojection view\n"
	    "\th - show/hide object histogram\n"
	    "\tp - pause video\n"
	    "To initialize tracking, select the object with mouse\n";
}
   

int main(int argc, char** argv)
{
    ros::init(argc, argv, "imageGet_node");  // ros初始化，定义节点名为imageGet_node
    ros::NodeHandle nh;                      // 定义ros句柄
    image_transport::ImageTransport it(nh);  //  类似ROS句柄
    image_transport::Publisher image_pub = it.advertise("/cameraImage", 1);   // 发布话题名/cameraImage
    ros::Publisher center_pub = nh.advertise<cv_with_car::center_x>("/center_x",100);
    ros::Time now = ros::Time::now();
    cv_with_car::center_x p;

    // ros::Rate loop_rate(200);   // 设置刷新频率，Hz
    
    showHelp();
	
    VideoCapture cap;
    Rect trackWindow;
    int hsize = 16;
    float hranges[] = {0,180};
    const float* phranges = hranges;
    //打开摄像机camera0
    cap.open(4);
    if( !cap.isOpened() )
    {
        cout << "***Could not initialize capturing...***\n";
    }
        
    // 显示窗口名称、注册鼠标回调函数、创建参数条
    namedWindow( "Histogram", 0 );
    namedWindow( "CamShift Demo", 0 );
    setMouseCallback( "CamShift Demo", onMouse, 0 );
    createTrackbar( "Vmin", "CamShift Demo", &vmin, 256, 0 );
    createTrackbar( "Vmax", "CamShift Demo", &vmax, 256, 0 );
    createTrackbar( "Smin", "CamShift Demo", &smin, 256, 0 );
 
    Mat frame, hsv, hue, mask, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
    bool paused = false;
    while (ros::ok){
    for(;;)
    {
        if( !paused )
        {
            cap >> frame;
            // waitKey(10);
            if( frame.empty() )
                break;
            
        }
 
        // 把frame的内容复制到image
        frame.copyTo(image);
 
        if( !paused )
        {
            // RGB转为HSV利于处理
            cvtColor(image, hsv, COLOR_BGR2HSV);
 
            if( trackObject )
            {
                int _vmin = vmin, _vmax = vmax;
 
                // inRange函数设置亮度阈值，去除低亮度的像素点的影响

                inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
                    Scalar(180, 256, MAX(_vmin, _vmax)), mask);
                int ch[] = { 0, 0 };
                hue.create(hsv.size(), hsv.depth());
                mixChannels(&hsv, 1, &hue, 1, ch, 1);
 
                if( trackObject < 0 )
                {
                    // Object has been selected by user, set up CAMShift search properties once
                    Mat roi(hue, selection), maskroi(mask, selection);
                    calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
                    normalize(hist, hist, 0, 255, NORM_MINMAX);
 
                    trackWindow = selection;
                    trackObject = 1; // Don't set up again, unless user selects new ROI
 
                    histimg = Scalar::all(0);
                    int binW = histimg.cols / hsize;
                    Mat buf(1, hsize, CV_8UC3);
                    for( int i = 0; i < hsize; i++ )
                        buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
                    cvtColor(buf, buf, COLOR_HSV2BGR);
 
                    for( int i = 0; i < hsize; i++ )
                    {
                        int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows/255);
                        rectangle( histimg, Point(i*binW,histimg.rows),
                                   Point((i+1)*binW,histimg.rows - val),
                                   Scalar(buf.at<Vec3b>(i)), -1, 8 );
                    }
                }
 
                // Perform CAMShift
                calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
                backproj &= mask;
                RotatedRect trackBox = CamShift(backproj, trackWindow,
                                    TermCriteria( TermCriteria::EPS | TermCriteria::COUNT, 10, 1 ));
                
                if (trackWindow.area() <= 1)
                {
                    int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
                    trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
                        trackWindow.x + r, trackWindow.y + r) &
                        Rect(0, 0, cols, rows);
                    
                }
 
                if( backprojMode )
                    cvtColor( backproj, image, COLOR_GRAY2BGR );
                
                ellipse( image, trackBox, Scalar(0,0,255), 3, LINE_AA );
                p.header.seq+=1;
                p.header.stamp=ros::Time::now();
                // p.imag_x=image.rows;
                p.imag_x=image.cols;
                p.x=trackBox.center.x;
                center_pub.publish(p);
                cout<<"ok"<<endl;
                
            }
        }
        else if( trackObject < 0 )
            paused = false;
 
        if( selectObject && selection.width > 0 && selection.height > 0 )
        {
            Mat roi(image, selection);
            bitwise_not(roi, roi);
        }
 
        cv::imshow( "CamShift Demo", image );
        cv::imshow( "Histogram", histimg );
        char c = (char)waitKey(10);
        if( c == 27 )
            break;
        switch(c)
        {
        case 'b':
            backprojMode = !backprojMode;
            break;
        case 'c':
            trackObject = 0;
            histimg = Scalar::all(0);
            break;
        case 'h':
            showHist = !showHist;
            if( !showHist )
                destroyWindow( "Histogram" );
            else
                namedWindow( "Histogram", 1 );
            break;
        case 'p':
            paused = !paused;
            break;
        default:
            ;
        }
    }
    
    return 0;
    }
}

