#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include "tcp_connection.h"
#include "speed_command_processor.h"

double wheel_distance; // 轮间距离，用于计算差分驱动的左右轮速度
double max_speed_value; // 最大速度值，用于归一化速度计算
int can_frame_dlc; // CAN帧的数据长度代码
int can_id_left_wheel; // 左轮电机的CAN ID
int can_id_right_wheel; // 右轮电机的CAN ID
int loop_rate; // 节点循环频率
std::string server_ip_str;  
int server_port; 

void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg, SpeedCommandProcessor& processor, TCPConnection& tcp) {
    processor.processCommand(msg, tcp);
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "cmd_vel_to_tcp_node");
    ros::NodeHandle nh;

    nh.param("wheel_distance", wheel_distance, 0.75);
    nh.param("max_speed_value", max_speed_value, 1.0);
    nh.param("can_frame_dlc", can_frame_dlc, 8);
    nh.param("can_id_left_wheel", can_id_left_wheel, 0x601);
    nh.param("can_id_right_wheel", can_id_right_wheel, 0x602);
    nh.param("loop_rate", loop_rate, 200);
    nh.param<std::string>("server_ip", server_ip_str, "10.0.0.10");
    nh.param("server_port", server_port, 7000);

    TCPConnection tcp(server_ip_str, server_port);

    try {
        if (!tcp.connectToServer()) {
            throw std::runtime_error("Failed to connect to server");
        }
        tcp.sendInitialMessages();
        tcp.startReceiving();
        
        SpeedCommandProcessor processor(wheel_distance, max_speed_value);
        ros::Subscriber cmd_vel_sub = nh.subscribe<geometry_msgs::Twist>("cmd_vel", 1000, 
            boost::bind(cmdVelCallback, _1, boost::ref(processor), boost::ref(tcp)));

        ros::Rate rate(loop_rate);
        while (ros::ok()) {
            ros::spinOnce();
            rate.sleep();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    ros::shutdown();
    return 0;
}
