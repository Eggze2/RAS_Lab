// my LIB
#include "servo_control.h"

servo::servo(ros::NodeHandle n_) {
    this->nh_ = n_;
    callback_flag = 0;
    servo_control_sub_=nh_.subscribe("/servo_control",1,&servo::servo_control_callback,this);
    serial_init();
}

void servo::serial_init() {
    try {
        ser.setPort("/dev/ttyUSB0");
        ser.setBaudrate(57600);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch(const std::exception& e) {
        ROS_INFO_STREAM("Failed to open port ");
    }
    ROS_INFO_STREAM("Succeed to open port");
}

void servo::servo_control_callback(const servo_control::servo_write msg) {
    msg_rec = msg;
    callback_flag = 1;
    std::cout << "接收到舵机控制指令" << std::endl;
}

void servo::servo_write_(servo_control::servo_write msg_) {
    unsigned char buf[3] = {0};
    char id[3]           = {0};
    char value[3]        = {0};

    if(callback_flag == 1) {
        // 类型转换
        sprintf(id,    "%d", msg_.id);                        // 将int8转换为char
        sprintf(value, "%d", msg_.value);                     // 将int8转换为char
        unsigned char* value1 =  (unsigned char*) value;
        unsigned char* id1    =  (unsigned char*) id;
        unsigned char* buf1   = buf;

        // 连接字符串
        char* str = (char *) malloc(20);                      // 分配内存,不然核心转储
        strcpy(str, "s ");
        strcat(str, id);
        strcat(str, " ");
        strcat(str, value);
        strcat(str, "\r");
        unsigned char* str_all = (unsigned char*)str;
        std::cout << str_all << std::endl;
        ser.write(str_all, sizeof(str_all));
        // 标志位清零
        callback_flag = 0;                                
    }
}

void servo::servo_handle() {
    servo_write_(msg_rec);
}

int main(int argc, char** argv)
{
    ros::init(argc,argv,"servo_control");
    ros::NodeHandle nh;
    servo servo(nh);
    
    ros::Duration(2).sleep();

    if (servo.ser.isOpen()) {
        ROS_INFO("Serial Port Opened!");
    }
    else {
        ROS_ERROR("Serial Port Cannot be Opened!");
    }

    unsigned char init[10]="s 0 81\r";
    servo.ser.write(init, sizeof(init));

    unsigned char init1[10]="s 1 70\r";
    servo.ser.write(init1, sizeof(init));
    while(ros::ok()) {
        ros::spinOnce();
        servo.servo_handle();
        if(servo.ser.available()) {
            ROS_INFO_STREAM("Reading from serial port");
            std_msgs::String result;
            ros::Duration(0.5).sleep();
            result.data = servo.ser.read(servo.ser.available());
            ROS_INFO_STREAM("Read: " << result.data);
        }
    }
    return 0;
}
