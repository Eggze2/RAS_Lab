cmake_minimum_required(VERSION 3.0.2)
project(straightNav)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  nav_core
  roscpp
  rospy
  std_msgs
  cv_bridge
  sensor_msgs
)
SET(EIGEN3_INCLUDE_DIR  /usr/include/eigen3)
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
include_directories(
  include ${catkin_INCLUDE_DIRS}
)

add_library(straightNav
  src/straightNav.cpp
)

add_library(pmf
  src/pmf.cpp
)

