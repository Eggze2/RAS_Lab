#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <unistd.h>

int main(int argc, char** argv) {
  ros::init(argc, argv, "cmd_vel_test_node");
  ros::NodeHandle nh;

  // 创建发布者，发布到 /cmd_vel 话题
  ros::Publisher cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1000);

  ros::Rate loop_rate(1);

  while (ros::ok()) {
    geometry_msgs::Twist msg;

    //z负值：前进
    //z正值：后退
    //x正值：逆时针
    //x负：顺时针
    msg.linear.x = 0.2;
    msg.angular.z = 0.3;
    ROS_INFO("Sending velocity command: linear.x=%.2f, angular.z=%.2f", 
              msg.linear.x, msg.angular.z);

    // 持续发送 2 秒
    ros::Time start_time = ros::Time::now();
    while (ros::ok() && (ros::Time::now() - start_time).toSec() < 2.0) { 
      cmd_vel_pub.publish(msg);
      loop_rate.sleep(); 
    }

    // 发送停止指令
    msg.linear.x = 0.0;
    msg.angular.z = 0.0;
    cmd_vel_pub.publish(msg); 
    ROS_INFO("Stop");
    sleep(20);

    // ... （其他测试指令） ... 
  }

  return 0;
}

