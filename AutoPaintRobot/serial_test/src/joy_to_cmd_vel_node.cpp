#include <ros/ros.h>
#include "joy_handler.h"

int main(int argc, char **argv) {
    ros::init(argc, argv, "joy_to_cmd_vel_node");
    ros::NodeHandle nh;

    JoyHandler joy_handler(nh);

    ros::Rate rate(200);

    while (ros::ok()) {
        joy_handler.publish();
        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}

