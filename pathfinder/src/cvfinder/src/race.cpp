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
#include "cvfinder/msg_card.h"
#include "cvfinder/center_x.h"
#include "std_msgs/Header.h"
#include <message_filters/sync_policies/approximate_time.h>
#include <ctime>
using namespace sensor_msgs;
using namespace message_filters;
using namespace std;
ros::Publisher pub;

void callback(const cvfinder::msg_cardConstPtr &f){
    // cout<< f->flag << endl;
    // cout<<"start"<<endl;
    geometry_msgs::Twist go;//正常情况下？
    go.linear.x =0.15;
    go.linear.y = 0.0;
    go.linear.z = 0.0;

    go.angular.x = 0.0;
    go.angular.y = 0.0;
    go.angular.z = 0.0;

    geometry_msgs::Twist go2;//正常情况下？
    go2.linear.x = -0.15;
    go2.linear.y = 0.0;
    go2.linear.z = 0.0;

    go2.angular.x = 0.0;
    go2.angular.y = 0.0;
    go2.angular.z = 0.0;

    geometry_msgs::Twist turn;//旋转
    turn.linear.x =0.0;
    turn.linear.y = 0.0;
    turn.linear.z = 0.0;

    turn.angular.x = 0.0;
    turn.angular.y = 0.0;
    turn.angular.z = 0.15;
   
    geometry_msgs::Twist turn2;//旋转
    turn2.linear.x =0.0;
    turn2.linear.y = 0.0;
    turn2.linear.z = 0.0;

    turn2.angular.x = 0.0;
    turn2.angular.y = 0.0;
    turn2.angular.z = -0.15;

    geometry_msgs::Twist stop;//旋转
    stop.linear.x =0.0;
    stop.linear.y = 0.0;
    stop.linear.z = 0.0;

    stop.angular.x = 0.0;
    stop.angular.y = 0.0;
    stop.angular.z = 0.0;
    int flag0 = f->flag;
    // if (flag0==0){
    //     pub.publish(turn);
    //     cout<<"turn"<<endl;
    // }else if(flag0 ==1 ){
    //     pub.publish(go);
    //     cout<<"go"<<endl;
    // }
    // else if(flag0 == 3){
    //     pub.publish(stop);
    //     cout<<"stop"<<endl;
    //     _exit(0);
    // }
    if (flag0==0){
        pub.publish(turn);
        cout<<"turn"<<endl;
    }else if(flag0 ==1 ){
        pub.publish(turn);
        cout<<"turn2"<<endl;
    }else if(flag0 == 2){
        pub.publish(turn2);
        cout<<"turn"<<endl;
    }
    else if(flag0 == 3){
        pub.publish(go);
        cout<<"gogogo"<<endl;
    }    
    else if(flag0 == 4){
        pub.publish(stop);
        cout<<"stop"<<endl;
        _exit(0);
}
    
}

int main(int argc, char *argv[])
{
    
    setlocale(LC_ALL,"");
    ros::init(argc,argv,"race");

    ros::NodeHandle nh;
    
    pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel",1);//发布
    ros::Subscriber sub = nh.subscribe<cvfinder::msg_card>("/flag_pub",1,callback);
    
    ros::spin();
    return 0;
}