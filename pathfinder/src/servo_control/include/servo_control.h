#pragma once
#ifndef SERVO_CONTROL
#define SERVO_CONTROL

// C++ LIB
#include <stdlib.h>
#include <string.h>
#include <serial/serial.h>
// ROS LIB
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <servo_control/servo_write.h>

// 数字舵机类
class servo {
private:
    // 句柄
    ros::NodeHandle nh_;
    // 订阅话题
    ros::Subscriber servo_control_sub_;
    // 自定义msg
    servo_control::servo_write msg_rec;
    // 标志位：是否接收到回调信息
    bool callback_flag;

public:
    // 创建seial对象
    serial::Serial ser;
    // 功能描述：构造函数
    servo(ros::NodeHandle n_);
    // 功能描述：回调函数
    void servo_control_callback(const servo_control::servo_write msg);
    // 功能描述：串口初始化
    void serial_init();
    // 功能描述：发送舵机编号和角度
    void servo_write_(servo_control::servo_write msg_);
    // 功能描述：执行函数
    void servo_handle();
};

#endif
