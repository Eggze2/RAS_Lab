#include "ros/ros.h"
#include <sensor_msgs/LaserScan.h>
#include "tf2_ros/static_transform_broadcaster.h"
#include "geometry_msgs/TransformStamped.h"
#include "tf2/LinearMath/Quaternion.h"
#include "geometry_msgs/Twist.h"
#include<string>
#include<iostream>
#include<geometry_msgs/Pose.h>
#include<nav_msgs/Odometry.h>
#include<time.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include "cv_with_car/center_x.h"
#include "cv_with_car/msg_card.h"
#include "std_msgs/Header.h"
#include <message_filters/sync_policies/approximate_time.h>
#include <ctime>
using namespace sensor_msgs;
using namespace message_filters;
using namespace std;
bool run = false;
bool follow = false;
ros::Publisher pub;
geometry_msgs::Twist go;
    
// ros::Duration du(1);//持续3秒钟,参数是double类型的，以秒为单位

void callback(const cv_with_car::center_xConstPtr  &center_sub, const sensor_msgs::LaserScanConstPtr  &laser_sub)
{   
    cout << laser_sub->ranges.size() <<endl;
    cout<<"start"<<endl;
    float g = laser_sub->ranges[720];
    // std::cout<<"jjjjj"<<std::endl;
    ROS_INFO("ATTENTION %f m", g);
    run = true;
    follow=true;
    for(int i=700; i<740; i++){
        if(laser_sub->ranges[i] < 0.5&&laser_sub->ranges[i] != 0){   //重要参数：距离，需要后续调参数
            run = false;
        }
    }
    int pre=center_sub->x;
    int mid= (center_sub ->imag_x)/2;
    if((pre - mid) >= -100 && (pre-mid)<= 100){
        follow = true;
    }else{
        follow = false;
    }
    //这里编一段关于质心的判断，偏离中间多少时follow=false 就开始转；当在什么区间范围内时，发布向前


    geometry_msgs::Twist back;
    back.linear.x =-0.10;
    back.linear.y = 0.0;
    back.linear.z = 0.0;

    back.angular.x = 0.0;
    back.angular.y = 0.0;
    back.angular.z = -0.5;

    geometry_msgs::Twist go;//正常情况下？
    go.linear.x =0.10;
    go.linear.y = 0.0;
    go.linear.z = 0.0;

    go.angular.x = 0.0;
    go.angular.y = 0.0;
    go.angular.z = 0.0;
    geometry_msgs::Twist turn;//旋转
    turn.linear.x =0.0;
    turn.linear.y = 0.0;
    turn.linear.z = 0.0;

    turn.angular.x = 0.0;
    turn.angular.y = 0.0;
    turn.angular.z = 0.1;
    geometry_msgs::Twist stop;//旋转
    turn.linear.x =0.0;
    turn.linear.y = 0.0;
    turn.linear.z = 0.0;

    turn.angular.x = 0.0;
    turn.angular.y = 0.0;
    turn.angular.z = 0.0;
    
    geometry_msgs::Twist turn2;//旋转
    turn.linear.x =0.0;
    turn.linear.y = 0.0;
    turn.linear.z = 0.0;

    turn.angular.x = 0.0;
    turn.angular.y = 0.0;
    turn.angular.z = -0.1;

    if(!run){
    //     //设置小车运动的时间
    //     int duration = 3; //运动时长，单位是秒
    //     ros::Time tstart = ros::Time::now(); //获取当前时间
    //     // 启动ROS节点程序的循环
    //     while(ros::ok()){
    //     // 获取当前时间
    //     ros::Time tcur = ros::Time::now();
    //     //计算小车已经运动的时间
    //     ros::Duration elapsedTime = tcur - tstart;
    //     // 判断小车已经运动的时间是否超过指定的时间
    //     if(elapsedTime.toSec() >= duration){
    //        // 小车停止运动
    //        pub.publish(stop);
    //        ROS_INFO("Stop the robot after %d seconds", duration);
    //        break;
    //    }else{
    //        // 小车继续向前运动
    //        pub.publish(turn);
    //        ROS_INFO("Move the robot forward after %f seconds", elapsedTime.toSec());
    //    }
    //    ros::spinOnce();
    //    }
    //     int duration1 = 3; //运动时长，单位是秒
    //     ros::Time tstart1 = ros::Time::now(); //获取当前时间

    //     // 启动ROS节点程序的循环
    //     while(ros::ok()){
    //     // 获取当前时间
    //     ros::Time tcur1 = ros::Time::now();

    //    //计算小车已经运动的时间
    //     ros::Duration elapsedTime1 = tcur1 - tstart1;

    //      // 判断小车已经运动的时间是否超过指定的时间
    //     if(elapsedTime1.toSec() >= duration1){
    //        // 小车停止运动
    //        pub.publish(stop);
    //        ROS_INFO("Stop the robot after %d seconds", duration1);
    //        break;
    //    }else{
    //        // 小车继续向前运动
    //        pub.publish(go);
    //        ROS_INFO("Move the robot forward after %f seconds", elapsedTime1.toSec());
    //    }
    //    ros::spinOnce();
    //    }
    //     pub.publish(back);
    //     ROS_INFO("back OK");
        ROS_INFO("back OK");
        pub.publish(stop);
    }else if (run && (!follow) && (pre - mid)<=0){
        ROS_INFO("follow2 OK");
        pub.publish(turn2);
    }else if (run && (!follow) && (pre - mid)>=0){
        ROS_INFO("follow OK");
        pub.publish(turn);
    }else if(run && follow){
        ROS_INFO("go OK");
        pub.publish(go);
    }
        
}


int main(int argc, char *argv[])
{
    
    setlocale(LC_ALL,"");
    ros::init(argc,argv,"lidar_node");

    ros::NodeHandle nh;
    
    pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel",10);//发布
    message_filters::Subscriber<cv_with_car::center_x> center_sub(nh,"/center_x",10);
    message_filters::Subscriber<sensor_msgs::LaserScan> laser_sub(nh,"/scan",10);
    // TimeSynchronizer <cv_with_car::center_x, sensor_msgs::LaserScan> sync(center_sub, laser_sub, 1000);
    typedef sync_policies::ApproximateTime<cv_with_car::center_x, sensor_msgs::LaserScan> MySyncPolicy;
    Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), center_sub, laser_sub);
    sync.registerCallback(boost::bind(&callback, _1, _2));
    ros::spin();
    return 0;
}