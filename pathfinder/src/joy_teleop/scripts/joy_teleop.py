#!/usr/bin/env python

import rospy
from sensor_msgs.msg import Joy
from geometry_msgs.msg import Twist

class JoyTeleop:
    def __init__(self):
        rospy.init_node('joy_teleop')

        self.pub = rospy.Publisher('/cmd_vel', Twist, queue_size=1)

        # Axes parameters
        self.linear_axis = rospy.get_param('~linear_axis', 1)
        self.angular_axis = rospy.get_param('~angular_axis', 3)

        # Speed scaling
        self.linear_scale = rospy.get_param('~linear_scale', 1.0)
        self.angular_scale = rospy.get_param('~angular_scale', 1.0)

        # Shoulder buttons for scaling
        self.linear_increase_btn = rospy.get_param('~linear_increase_btn', 4)
        self.linear_decrease_axis = rospy.get_param('~linear_decrease_axis', 2)
        self.angular_increase_btn = rospy.get_param('~angular_increase_btn', 5)
        self.angular_decrease_axis = rospy.get_param('~angular_decrease_axis', 5)

        # Initialize previous values for comparison
        self.prev_linear_speed = 0.0
        self.prev_angular_speed = 0.0

        # Initialize the Twist message
        self.twist = Twist() 

        rospy.Subscriber('/joy', Joy, self.joy_callback)

        # Set up a timer to publish Twist messages at a rate of 10Hz
        self.timer = rospy.Timer(rospy.Duration(0.1), self.timer_callback) 

        rospy.loginfo("Joy Teleop node started")

    def joy_callback(self, data):
        # Adjust linear speed based on shoulder buttons and triggers
        linear_speed = self.linear_scale
        if data.buttons[self.linear_increase_btn] == 1:
            linear_speed *= 2.0
        if data.axes[self.linear_decrease_axis] < 0:
            linear_speed *= 0.5

        # Adjust angular speed based on shoulder buttons and triggers
        angular_speed = self.angular_scale
        if data.buttons[self.angular_increase_btn] == 1:
            angular_speed *= 2.0
        if data.axes[self.angular_decrease_axis] < 0:
            angular_speed *= 0.5

        # Calculate the target speeds based on joystick input
        target_linear_speed = data.axes[self.linear_axis] * linear_speed
        target_angular_speed = data.axes[self.angular_axis] * angular_speed

        # Update the Twist message with the latest speed values
        self.twist.linear.x = target_linear_speed
        self.twist.angular.z = target_angular_speed

    def timer_callback(self, event):
        # Publish the Twist message every time the timer triggers
        self.pub.publish(self.twist)

if __name__ == '__main__':
    try:
        JoyTeleop()
        rospy.spin()
    except rospy.ROSInterruptException:
        pass