#ifndef SPEED_COMMAND_PROCESSOR_H
#define SPEED_COMMAND_PROCESSOR_H

#include <geometry_msgs/Twist.h>
#include "tcp_connection.h"

union SpeedData {
    int16_t value;
    uint8_t bytes[2];
};

class SpeedCommandProcessor {
public:
    SpeedCommandProcessor(double wheel_dist, double max_speed);
    void processCommand(const geometry_msgs::Twist::ConstPtr& msg, TCPConnection& tcp);

private:
    double wheel_distance;
    double max_speed_value;
    double prev_v;
    double prev_omega;
};

#endif // SPEED_COMMAND_PROCESSOR_H
