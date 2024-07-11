#include "serial_handler.h"
#include <sstream>
#include <algorithm>

SerialHandler::SerialHandler(const std::string& port, uint32_t baud_rate)
    : pump_duty(0), last_pump_duty(-1) {
    ser.setPort(port);
    ser.setBaudrate(baud_rate);
    serial::Timeout to = serial::Timeout::simpleTimeout(1000);
    ser.setTimeout(to);

    try {
        ser.open();
    } catch (serial::IOException& e) {
        ROS_ERROR_STREAM("Unable to open port");
        throw e;
    }

    if (ser.isOpen()) {
        ROS_INFO_STREAM("Serial Port initialized");
    } else {
        throw std::runtime_error("Failed to open serial port");
    }
}

void SerialHandler::customControlCallback(const serial_test::CustomControlMsg::ConstPtr& msg) {
    int x_axis = msg->axis6;
    int y_axis = msg->axis7;
    int button0 = msg->button0;
    int button1 = msg->button1;
    int button2 = msg->button2;

    int speed = 2000;
    int mode = 1;

    if (x_axis != 0 || y_axis != 0) {
        std::ostringstream move_cmd;
        move_cmd << "move " << x_axis << " " << y_axis << " " << speed << " " << mode << "\r\n";
        std::string move_cmd_str = move_cmd.str();
        try {
            ser.write(move_cmd_str);
            ROS_INFO_STREAM("Sent move command: " << move_cmd_str);
        } catch (const std::exception& e) {
            ROS_ERROR_STREAM("Failed to send move command: " << e.what());
            if (!ser.isOpen()) {
                ROS_ERROR_STREAM("Reopening serial port...");
                try {
                    ser.open();
                    ROS_INFO_STREAM("Serial Port reopened");
                } catch (const std::exception& e) {
                    ROS_ERROR_STREAM("Failed to reopen serial port: " << e.what());
                }
            }
        }
    }

    if (button0 == 1) {
        pump_duty = 0;
    } else if (button2 == 1) {
        pump_duty = std::min(pump_duty + 10, 100);
    } else if (button1 == 1) {
        pump_duty = std::max(pump_duty - 10, 0);
    }

    if (pump_duty != last_pump_duty) {
        std::ostringstream pump_cmd;
        pump_cmd << "pump " << pump_duty;
        std::string pump_cmd_str = pump_cmd.str();
        try {
            ser.write(pump_cmd_str + "\r\n");
            ROS_INFO_STREAM("Sent pump command: " << pump_cmd_str);
            last_pump_duty = pump_duty;
        } catch (const std::exception& e) {
            ROS_ERROR_STREAM("Failed to send pump command: " << e.what());
            if (!ser.isOpen()) {
                ROS_ERROR_STREAM("Reopening serial port...");
                try {
                    ser.open();
                    ROS_INFO_STREAM("Serial Port reopened");
                } catch (const std::exception& e) {
                    ROS_ERROR_STREAM("Failed to reopen serial port: " << e.what());
                }
            }
        }
    }
}
