cmake_minimum_required(VERSION 2.8.3)
project(servo_control)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  roscpp
  rospy
  sensor_msgs
  tf
  serial
  std_msgs
  message_generation
  nav_msgs
)

#add_message_files(FILES
#  my_msg.msg
#)
add_message_files(
  FILES 
  servo_write.msg
)

#generate_messages(DEPENDENCIES
#  std_msgs
#)
generate_messages(
  DEPENDENCIES 
  std_msgs
)

catkin_package(
  CATKIN_DEPENDS
  roscpp rospy std_msgs message_runtime
)

# catkin_package(
#    INCLUDE_DIRS include
# )

include_directories(
  ${catkin_INCLUDE_DIRS}
  include
)

add_executable(servo_control src/servo_control.cpp)

add_dependencies(servo_control ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

target_link_libraries(servo_control ${catkin_LIBRARIES})




