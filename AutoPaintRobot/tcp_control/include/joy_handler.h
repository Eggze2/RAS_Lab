#ifndef JOY_HANDLER_H
#define JOY_HANDLER_H

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>

class JoyHandler {
public:
    JoyHandler(ros::NodeHandle& nh);
    void joyCallback(const sensor_msgs::Joy::ConstPtr& msg);
    void publish();

private:
    ros::NodeHandle nh_;
    ros::Publisher cmd_vel_pub_;
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
};

#endif // JOY_HANDLER_H
