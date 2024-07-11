#include <ros/ros.h>
#include "joy_handler.h"
#include "serial_handler.h"

int main(int argc, char** argv) {
    ros::init(argc, argv, "joy_to_serial_node");
    ros::NodeHandle nh;

    JoyHandler joyHandler(nh);
    SerialHandler serialHandler("/dev/ttyUSB0", 115200);

    ros::Subscriber custom_control_sub = nh.subscribe<serial_test::CustomControlMsg>("custom_control_topic", 1000, &SerialHandler::customControlCallback, &serialHandler);

    ros::Rate rate(200);
    while (ros::ok()) {
        joyHandler.publish();
        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}
