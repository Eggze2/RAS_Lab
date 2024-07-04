#include "speed_command_processor.h"
#include <algorithm>

SpeedCommandProcessor::SpeedCommandProcessor(double wheel_dist, double max_speed)
    : wheel_distance(wheel_dist), max_speed_value(max_speed), prev_v(0.0), prev_omega(0.0) {}

void SpeedCommandProcessor::processCommand(const geometry_msgs::Twist::ConstPtr& msg, TCPConnection& tcp) {
    double v = -msg->linear.x;
    double omega = msg->angular.z;
    
    if (v == prev_v && omega == prev_omega) {
        return;
    }

    prev_v = v;
    prev_omega = omega;

    double v_l = -(v - omega * wheel_distance / 2);
    double v_r = v + omega * wheel_distance / 2;

    int duty_cycle_left = static_cast<int>((v_l / max_speed_value) * 1000);
    int duty_cycle_right = static_cast<int>((v_r / max_speed_value) * 1000);

    duty_cycle_left = std::max(std::min(duty_cycle_left, 1000), -1000);
    duty_cycle_right = std::max(std::min(duty_cycle_right, 1000), -1000);

    SpeedData left_duty_cycle, right_duty_cycle;
    left_duty_cycle.value = duty_cycle_left;
    right_duty_cycle.value = duty_cycle_right;

    uint8_t data_l[13] = {0x08, 0x00, 0x00, 0x06, 0x01, 0x2B, 0x01, 0x20, 0x00, left_duty_cycle.bytes[0], left_duty_cycle.bytes[1], 0x00, 0x00};
    uint8_t data_r[13] = {0x08, 0x00, 0x00, 0x06, 0x02, 0x2B, 0x01, 0x20, 0x00, right_duty_cycle.bytes[0], right_duty_cycle.bytes[1], 0x00, 0x00};

    tcp.sendData(data_l, sizeof(data_l));
    tcp.sendData(data_r, sizeof(data_r));
}
