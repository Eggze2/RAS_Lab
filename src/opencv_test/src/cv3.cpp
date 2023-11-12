#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
using namespace cv;
using namespace std;

// void imageCallback(const sensor_msgs::ImageConstPtr& msg)
// {
//     try   // 如果转换失败，则提跳转到catch语句
//     {
//         cv::imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);   // 将图像转换openCV的格式，并输出到窗口
//         cv::waitKey(0); // 一定要有wiatKey(),要不然是黑框或者无窗口     
//     }
//     catch(cv_bridge::Exception& e)
//     {
//         ROS_ERROR("Could not convert for '%s' to 'bgr8'.", msg->encoding.c_str());
//     }
    
// }

// int main(int argc, char** argv)
// {
//     ros::init(argc, argv, "imageSub_node");   // 注册节点名 
//     ros::NodeHandle nh; // 注册句柄
//     image_transport::ImageTransport it(nh); // 注册句柄
//     image_transport::Subscriber imageSub = it.subscribe("/cameraImage", 1, imageCallback);  // 订阅/cameraImage话题，并添加回调函数
//     ros::spin();  // 循环等待回调函数触发
// }


// int main(int argc, char* argv[])
// {
// 	//轮廓检测
// 	vector<vector<Point>> contours;
// 	vector<Vec4i> hierarchy;
//     ros::init(argc, argv, "ima");  // ros初始化，定义节点名为imageGet_node
//     ros::NodeHandle nh;                      // 定义ros句柄
//     image_transport::ImageTransport it(nh);  //  类似ROS句柄
// 	//Create the capture object
// 	//0->input arg that specifies it should take the input from the webcam
// 	VideoCapture cap(0);
// 	if (!cap.isOpened()) {
// 		cerr << "Unable to open the webcam. Exiting!" << endl;
// 		return -1;
// 	}
// 	Mat frame, hsvImage, mask, outputImage;
// 	char ch;
// 	//Image size scaling factor for the input frames from the webcam
// 	float scalingFactor = 0.75;
// 	//Variable declarations and initializations
// 	//Iterate until the user presses the Esc key
// 	while(true && nh.ok()) {
// 		//Initialize the output image before each iteration
// 		outputImage = Scalar(0, 0, 0);
// 		//Capture the current frame
// 		cap >> frame;
// 		//Check if 'frame' is empty
// 		if (frame.empty()){
// 			break;}
// 		//Resize the frame
// 		resize(frame, frame, Size(), scalingFactor, scalingFactor, INTER_AREA);
// 		//Convert to HSV colorspace
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

// 		imshow("Input", frame);}
// }

int main(int argc, char** argv)
{
    ros::init(argc, argv, "imageGet_node");  // ros初始化，定义节点名为imageGet_node
    ros::NodeHandle nh;  
    image_transport::ImageTransport it(nh);  //  类似ROS句柄
    image_transport::Publisher image_pub = it.advertise("/cameraImage", 1);   // 发布话题名/cameraImage
    // 定义ros句柄

    Mat temp = imread("door2.jpeg");
    Mat src = imread("door1.jpeg");
    Mat dst = src.clone();
    cv::imshow("temp", temp);
	//1.构建结果图像resultImg(注意大小和类型)
    //如果原图(待搜索图像)尺寸为W x H, 而模版尺寸为 w x h, 则结果图像尺寸一定是(W-w+1)x(H-h+1)
    //结果图像必须为单通道32位浮点型图像
    
    // int width = src.cols - temp.cols + 1;//result宽度
    // int height = src.rows - temp.rows + 1;//result高度
    // Mat result(height, width, CV_32FC1);//创建结果映射图像
    // //2.模版匹配
    // //这里我们使用的匹配算法是标准平方差匹配 method=TM_SQDIFF_NORMED，数值越小匹配度越好
    // //matchTemplate(src, temp, result, TM_SQDIFF); //平方差匹配法(最好匹配0)
    // matchTemplate(src, temp, result, TM_SQDIFF_NORMED); //归一化平方差匹配法(最好匹配0)
    // //matchTemplate(src, temp, result, TM_CCORR); //相关匹配法(最坏匹配0)
    // //matchTemplate(src, temp, result, TM_CCORR_NORMED); //归一化相关匹配法(最坏匹配0)
    // //matchTemplate(src, temp, result, TM_CCOEFF); //系数匹配法(最好匹配1)
    // //matchTemplate(src, temp, result, TM_CCOEFF_NORMED);//化相关系数匹配,最佳值1
    // imshow("result", result);
    // //3.正则化(归一化到0-1)
    // normalize(result, result, 0, 1, NORM_MINMAX, -1);//归一化到0-1范围
    // //4.找出result中的最大值及其位置
    // double minValue, maxValue;
    // Point minLoc, maxLoc;
    // // 定位极值的函数
    // minMaxLoc(result, &minValue, &maxValue, &minLoc, &maxLoc);
    // // cout << "minValue=" << minValue << endl;
    // // cout << "maxValue=" << maxValue << endl;
	// //5.根据result中的最大值位置在源图上画出矩形和中心点
    // rectangle(dst, maxLoc, Point(maxLoc.x + temp.cols, maxLoc.y + temp.rows), Scalar(0, 255, 0), 2, 8);
    // cv::imshow("dst", dst);
    
    waitKey(0);
    return 0;
}
