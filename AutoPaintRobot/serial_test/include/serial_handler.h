#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <ros/ros.h>
#include <serial/serial.h>
#include <serial_test/CustomControlMsg.h>

class SerialHandler {
public:
    SerialHandler(const std::string& port, uint32_t baud_rate);
    void customControlCallback(const serial_test::CustomControlMsg::ConstPtr& msg);

private:
    serial::Serial ser;
    int pump_duty;
    int last_pump_duty;
};

#endif // SERIAL_HANDLER_H
