#ifndef JOY_HANDLER_H
#define JOY_HANDLER_H

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>
#include <serial_test/CustomControlMsg.h>

class JoyHandler {
public:
    JoyHandler(ros::NodeHandle& nh);
    void joyCallback(const sensor_msgs::Joy::ConstPtr& msg);
    void publish();
    const serial_test::CustomControlMsg& getCustomMsg() const;

private:
    ros::NodeHandle nh_;
    ros::Publisher cmd_vel_pub_;
    ros::Publisher custom_control_pub_;
    ros::Subscriber joy_sub_;

    double max_linear_speed_;
    double min_speed_threshold_;
    double wheel_distance_;
    double joystick_deadzone_;
    double min_angular_velocity_;
    double scale_factor_;
    double linear_scale_exponent_;
    double angular_scale_exponent_;
    double speed_smoothing_factor_;
    geometry_msgs::Twist cmd_vel_;
    serial_test::CustomControlMsg custom_msg_;
};

#endif // JOY_HANDLER_H
