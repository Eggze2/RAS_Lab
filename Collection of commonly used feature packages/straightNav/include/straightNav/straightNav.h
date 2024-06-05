#ifndef STRAIGHTNAV_H
#define STRAIGHTNAV_H

#include <ros/ros.h>
#include <costmap_2d/costmap_2d_ros.h>
#include <costmap_2d/costmap_2d.h>
#include <nav_core/base_global_planner.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Path.h>

#include <fstream>

#include <vector>
#include <queue>
#define infinity 1.0e10
#define neutral_cost_ 50
using namespace std;

struct Node{
  float cost;
  unsigned int index;
};

namespace nav {
    class straightNav: public nav_core::BaseGlobalPlanner{
        public:
            straightNav(){}
            straightNav(std::string name, costmap_2d::Costmap2DROS* costmap_ros);
            
       
      void initialize(std::string name, costmap_2d::Costmap2DROS* costmap_ros);

     
      bool makePlan(const geometry_msgs::PoseStamped& start, 
          const geometry_msgs::PoseStamped& goal, std::vector<geometry_msgs::PoseStamped>& plan);
      
      unsigned int width;
      unsigned int height;
      unsigned int map_size;
      vector<bool> OGM;
      vector<bool> OGM_laser;
      double getHeuristic(int cell_index, int goal_index);
      
      vector<unsigned int> get_neighbors(unsigned int current_cell);
      double getMoveCost(unsigned int firstIndex,unsigned int secondIndex);

      bool isInBounds(unsigned int x,unsigned int y);   
      void publishPlan(const std::vector<geometry_msgs::PoseStamped>& path);
      void sublaser(const sensor_msgs::LaserScan::ConstPtr& p);
      
      double mul_mul(double k,double m);
      int C_(int n,int i);
      int binomialCoefficient(int n, int k);

      ros::Publisher plan_pub_;
      ros::Publisher plan_original;
      ros::Subscriber object_sub;
      
      std::string frame_id_;
	    bool initialized_;
	    costmap_2d::Costmap2DROS* costmap_ros_;
	    costmap_2d::Costmap2D* costmap_;
      ros::NodeHandle private_nh1;
    
      void circle(unsigned int current_cell);
      vector<float> cost_potential;
      vector<float> cost_potential_laser;
      float return_max(float a,float b);
    };
};

#endif
  
