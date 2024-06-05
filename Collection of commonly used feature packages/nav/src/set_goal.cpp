// #include <ros/ros.h>
// #include <move_base_msgs/MoveBaseAction.h>
// #include <actionlib/client/simple_action_client.h>
 
// typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
 
// int main(int argc, char** argv){
//   ros::init(argc, argv, "simple_navigation_goals");
 
//   //tell the action client that we want to spin a thread by default
//   MoveBaseClient ac("move_base", true);
 
//   //wait for the action server to come up
//   while(!ac.waitForServer(ros::Duration(5.0))){
//     ROS_INFO("Waiting for the move_base action server to come up");
//   }
 
//   move_base_msgs::MoveBaseGoal goal;
 
//   //we'll send a goal to the robot to move 1 meter forward
//   goal.target_pose.header.frame_id = "map";
//   goal.target_pose.header.stamp = ros::Time::now();
 
//   goal.target_pose.pose.position.x = 0.5;
//   goal.target_pose.pose.position.y = 0;
//   goal.target_pose.pose.orientation.w = 0.0;
 
//   ROS_INFO("Sending goal");
//   ac.sendGoal(goal);
 
//   ac.waitForResult();

//   ROS_INFO("50");

//   if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
//     ROS_INFO("Hooray, the base moved forward");
//   else
//     ROS_INFO("来自move_base的嘲讽");
 
//   return 0;
// }

#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <move_base_msgs/MoveBaseGoal.h>
#include <geometry_msgs/PoseStamped.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> Client;
int main(int argc, char** argv)
{
  ros::init(argc, argv, "move_base_actionlib_bing");

  Client client("move_base", true); // true -> don't need ros::spin()
  client.waitForServer(); // Waits for the ActionServer to connect to this client
  // Fill in goal here (0.813, 2.349, 0.000)
  printf("Server connected. \r\n");
  move_base_msgs::MoveBaseGoal action_goal;
  action_goal.target_pose.header.stamp = ros::Time::now();
  action_goal.target_pose.header.frame_id = "base_link";

  action_goal.target_pose.pose.position.x = 5;
  action_goal.target_pose.pose.position.y =-1;
  action_goal.target_pose.pose.position.z =0;

  action_goal.target_pose.pose.orientation.x =0;
  action_goal.target_pose.pose.orientation.y =0;
  action_goal.target_pose.pose.orientation.z =0;
  action_goal.target_pose.pose.orientation.w =1;

  client.sendGoal(action_goal); // Sends a goal to the ActionServer
  client.waitForResult(ros::Duration(05.0)); // Blocks until this goal finishes
  ros::Rate loop_rate(1);
  while (ros::ok()){
    printf("Current State: %s\n", client.getState().toString().c_str());
    loop_rate.sleep();
    int a;
    std::cout<<a++<<std::endl;

  }
  return 0;
}