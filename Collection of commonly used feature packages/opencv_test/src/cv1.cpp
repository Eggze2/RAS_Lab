#include <iostream>
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>  // 将ROS下的sensor_msgs/Image消息类型转化为cv::Mat数据类型
#include <sensor_msgs/image_encodings.h> // ROS下对图像进行处理
#include <image_transport/image_transport.h> // 用来发布和订阅图像信息

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    
    ros::init(argc, argv, "imageGet_node");  // ros初始化，定义节点名为imageGet_node
    ros::NodeHandle nh;                      // 定义ros句柄
    image_transport::ImageTransport it(nh);  //  类似ROS句柄
    image_transport::Publisher image_pub = it.advertise("/cameraImage", 1);   // 发布话题名/cameraImage

    ros::Rate loop_rate(200);   // 设置刷新频率，Hz

    cv::Mat imageRaw;  // 原始图像保存
    cv::VideoCapture capture(0);   // 创建摄像头捕获，并打开摄像头0(一般是0,2....)

    if(capture.isOpened() == 0)      // 如果摄像头没有打开
    {
        std::cout << "Read camera failed!" << std::endl;
        return -1;
    }

    while(nh.ok())
    {
        capture.read(imageRaw);          // 读取当前图像到imageRaw
        cv::imshow("veiwer", imageRaw);  // 将图像输出到窗口
        sensor_msgs::ImagePtr  msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", imageRaw).toImageMsg();  // 图像格式转换
        image_pub.publish(msg);         // 发布图像信息
        ros::spinOnce();                // 没什么卵用，格式像样
        loop_rate.sleep();              // 照应上面设置的频率
        if(cv::waitKey(2) >= 0){        // 延时ms,按下任何键退出(必须要有waitKey，不然是看不到图像的
			system("rosrun opencv_test cv3");
            break;     
        }     
    }
    
}

// int main(int argc, char* argv[])
// {	
// 	// system("rosrun opencv_test cv2");
// 	//轮廓检测
// 	vector<vector<Point>> contours;
// 	vector<Vec4i> hierarchy;
//     ros::init(argc, argv, "imageGet_node");  // ros初始化，定义节点名为imageGet_node
//     ros::NodeHandle nh;                      // 定义ros句柄
//     image_transport::ImageTransport it(nh);  //  类似ROS句柄
// 	//Create the capture object
// 	//0->input arg that specifies it should take the input from the webcam
// 	VideoCapture cap(2);
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

// 		imshow("Input", frame);
// 		imshow("Output", outputImage);
// 		Mat outputImage1 ;	//最小外接正矩形画布		
// 		cvtColor(outputImage,outputImage1,COLOR_BGR2GRAY);
// 		threshold(outputImage1,outputImage1, 80, 255, THRESH_BINARY);
// 		findContours(outputImage1, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());//寻找最外层轮廓
// 		Mat imageContours0 = Mat::zeros(outputImage.size(), CV_8UC1);	//最小外接正矩形画布
		
// 		vector<double> g_dConArea(contours.size());
// 		for (int i = 0; i < contours.size(); i++)
// 		{
// 			//绘制轮廓
// 			/*drawContours(imageContours0, contours, i, Scalar(255), 1, 8, hierarchy);*/
// 			//计算轮廓的面积
// 			g_dConArea[i] = contourArea(contours[i]);
// 			cout << "【用轮廓面积计算函数计算出来的第" << i << "个轮廓的面积为：】" << g_dConArea[i] << endl;
// 		}
// 		//寻找面积最大的部分
// 		int max = 0;
// 		for (int i = 1; i < contours.size(); i++) {
// 			if (g_dConArea[i] > g_dConArea[max]) {
// 				max = i;
// 			}
// 		}
// 		//输出最大面积即索引
// 		cout << "max=" << g_dConArea[max] << endl;
// 		cout << "idx=" << max << endl;
// 		//绘制轮廓
// 		drawContours(imageContours0, contours, max, Scalar(255), 1, 8, hierarchy);
// 		imshow("out1",imageContours0);

// 		ch = waitKey(30);
// 		if (ch == 27){
// 			break;
// 		}
// 	}
// 	cap.release();
// 	destroyAllWindows();	
// 	return 0;
// }


// //轮廓检测
// 		vector<vector<Point>> contours;   //轮廓
// 		vector<Vec4i> hieracrchy;    //轮廓结构
// 		findContours(outputImage, contours, hieracrchy, 3, 2, Point());   //检测最外围轮廓

// 		for (int i = 0; i < contours.size(); i++)
// 		{
// 			Rect rect = boundingRect(contours[i]);    //最大外接矩形
// 			//在最大外接矩形的外侧画一个更大一点的红色矩形框
// 			//rect是Rect类的一个对象，因此可以调用rect.x和rect.height等参数
// 			int x = rect.x - 3;    //更大矩形左上角顶点的x坐标
// 			int y = rect.y - 3;    //y坐标
// 			int height = rect.height + 6;    //高
// 			int width = rect.width + 6;      //宽
// 			rectangle(frame, Point(x, y), Point(x + width, y + height), Scalar(0,0, 0), 2);    //用左上和右下两个顶点坐标画矩形
// 		}