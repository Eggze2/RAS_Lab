#include "joy_handler.h"
#include <algorithm>
#include <ros/ros.h>

JoyHandler::JoyHandler(ros::NodeHandle& nh)
    : nh_(nh), max_linear_speed_(0.1), min_speed_threshold_(0.1), 
      wheel_distance_(0.75), joystick_deadzone_(0.05), 
      min_angular_velocity_(0.1), scale_factor_(0.5), 
      linear_scale_exponent_(0.5), angular_scale_exponent_(0.5), 
      speed_smoothing_factor_(0.2) {

    // 从参数服务器获取参数
    nh_.param("/joy_to_cmd_vel_node/max_linear_speed", max_linear_speed_, 0.1);
    nh_.param("/joy_to_cmd_vel_node/min_speed_threshold", min_speed_threshold_, 0.1);
    nh_.param("/joy_to_cmd_vel_node/wheel_distance", wheel_distance_, 0.75);

    cmd_vel_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
    custom_control_pub_ = nh_.advertise<serial_test::CustomControlMsg>("custom_control_topic", 1000);
    joy_sub_ = nh_.subscribe("joy", 1000, &JoyHandler::joyCallback, this);
}

void JoyHandler::joyCallback(const sensor_msgs::Joy::ConstPtr& msg) {
    // 根据手柄按键调整最大线速度和最小角速度
    if (msg->buttons[4] == 1) {
        max_linear_speed_ += 0.1;
        ROS_INFO("Max linear speed increased to: %f", max_linear_speed_);
    }
    if (msg->axes[2] == -1.0) {
        max_linear_speed_ -= 0.1;
        ROS_INFO("Max linear speed decreased to: %f", max_linear_speed_);
    }
    max_linear_speed_ = std::max(0.1, max_linear_speed_);

    if (msg->buttons[5] == 1) {
        min_angular_velocity_ += 0.1;
        ROS_INFO("Min angular velocity increased to: %f", min_angular_velocity_);
    }
    if (msg->axes[5] == -1.0) {
        min_angular_velocity_ -= 0.1;
        ROS_INFO("Min angular velocity decreased to: %f", min_angular_velocity_);
    }
    min_angular_velocity_ = std::max(0.1, min_angular_velocity_);

    double linear_vel = msg->axes[1] * max_linear_speed_;
    double angular_vel = msg->axes[3] * min_angular_velocity_;

    cmd_vel_.linear.x = linear_vel;
    cmd_vel_.angular.z = angular_vel;

    // 更新自定义消息
    custom_msg_.axis6 = -msg->axes[6] * 2000; // X轴滑台需要运动的步数，乘以-1来矫正方向
    custom_msg_.axis7 = msg->axes[7] * 5000;  // Y轴滑台需要运动的步数
    custom_msg_.button0 = msg->buttons[0];
    custom_msg_.button1 = msg->buttons[1];
    custom_msg_.button2 = msg->buttons[2];

    // 打印手柄按钮和摇杆状态
    ROS_INFO("Joystick Axes: [%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]", 
             msg->axes[0], msg->axes[1], msg->axes[2], msg->axes[3], 
             msg->axes[4], msg->axes[5], msg->axes[6], msg->axes[7]);
    ROS_INFO("Joystick Buttons: [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]", 
             msg->buttons[0], msg->buttons[1], msg->buttons[2], msg->buttons[3], 
             msg->buttons[4], msg->buttons[5], msg->buttons[6], msg->buttons[7], 
             msg->buttons[8], msg->buttons[9], msg->buttons[10], msg->buttons[11]);
}

void JoyHandler::publish() {
    cmd_vel_pub_.publish(cmd_vel_);
    custom_control_pub_.publish(custom_msg_);
}

const serial_test::CustomControlMsg& JoyHandler::getCustomMsg() const {
    return custom_msg_;
}
