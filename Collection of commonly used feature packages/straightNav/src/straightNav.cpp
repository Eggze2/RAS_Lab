#include"../include/straightNav/straightNav.h"
#include <geometry_msgs/PoseStamped.h>
#include<pluginlib/class_list_macros.h>
#include <tf/transform_broadcaster.h>
#include <mutex>
#include <cmath>

using std::cout;
using std::endl;
using std::vector;
mutex mtx;


PLUGINLIB_EXPORT_CLASS(nav::straightNav, nav_core::BaseGlobalPlanner);

using namespace std;


namespace nav
{
        straightNav::straightNav(std::string name, costmap_2d::Costmap2DROS* costmap_ros)
        {
            initialize(name, costmap_ros);
        }

        //导入地图
        void straightNav::initialize(std::string name, costmap_2d::Costmap2DROS* costmap_ros)
        {
            if(!initialized_){
            costmap_ros_ = costmap_ros;
            costmap_ = costmap_ros_->getCostmap();
            width = costmap_->getSizeInCellsX();
            height = costmap_->getSizeInCellsY();
            map_size = width * height;
            OGM.resize(map_size);
            OGM_laser.resize(map_size);
            cost_potential.resize(map_size);
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    unsigned int cost = static_cast<int>(costmap_->getCost(j,i));                 
                    if (cost ==0){
                        OGM[i * width + j] = true;
                    }
                    else{
                        OGM[i * width + j] = false;
                    }
                }
            }

            frame_id_ = costmap_ros->getGlobalFrameID();
            
            ros::NodeHandle private_nh("~/" + name);
            ros::NodeHandle private_original("~/" + name);
            plan_pub_ = private_nh.advertise<nav_msgs::Path>("plan", 1);
            plan_original = private_original.advertise<nav_msgs::Path>("plan_original",1);
            initialized_ = true;
            }
            else
                ROS_WARN("This planner has already been initialized... doing nothing");

            ROS_ERROR("initalize finish");
        }

        bool straightNav::makePlan(const geometry_msgs::PoseStamped &    start,
                        const geometry_msgs::PoseStamped &  goal,
                        std::vector< geometry_msgs::PoseStamped > &     plan )
                        {
                        if(!initialized_){
                            ROS_ERROR("The planner has not been initialized, please call initialize() to use the planner");
                            return false;
                        }
        // object_sub=private_nh1.subscribe("/scan",1,&straightNav::sublaser,this);
        ROS_INFO("Got a start: %.2f, %.2f, and a goal: %.2f, %.2f", start.pose.position.x, start.pose.position.y, goal.pose.position.x,goal.pose.position.y);
        double wx = start.pose.position.x;
        double wy = start.pose.position.y;

        unsigned int start_x, start_y;
        costmap_->worldToMap(wx, wy, start_x, start_y);
        unsigned int start_index = costmap_->getIndex(start_x, start_y);
        ROS_ERROR("get start_ps");
        wx = goal.pose.position.x;
        wy = goal.pose.position.y;

        unsigned int goal_x, goal_y;
        costmap_->worldToMap(wx, wy, goal_x, goal_y);
        unsigned int goal_index = costmap_->getIndex(goal_x, goal_y);
        ROS_ERROR("get goal");

        vector<float> gCosts(map_size, infinity);
        vector<unsigned int> cameFrom(map_size, -1);
        
        //？？？？？？？？开集
        multiset<Node> priority_costs;
        
        gCosts[start_index] = 0;
        
        Node currentNode;
        currentNode.index = start_index;
        currentNode.cost = gCosts[start_index] + 0;
        priority_costs.insert(currentNode);
        
        plan.clear();
        ROS_ERROR("plan.clear");
        while(!priority_costs.empty())
        {
            currentNode = *priority_costs.begin();
            priority_costs.erase(priority_costs.begin());
            if (currentNode.index == goal_index){
                break;
            }
            vector<unsigned int> neighborIndexes = get_neighbors(currentNode.index);
            
            for(int i = 0; i < neighborIndexes.size(); i++){
                if(cameFrom[neighborIndexes[i]] == -1){
                    gCosts[neighborIndexes[i]] = gCosts[currentNode.index] + getMoveCost(currentNode.index, neighborIndexes[i]);
                    Node nextNode;
                    nextNode.index = neighborIndexes[i];
                    // nextNode.cost = gCosts[neighborIndexes[i]];    //Dijkstra Algorithm
                    //                                           关于nextNode的启发函数
                    nextNode.cost = gCosts[neighborIndexes[i]] + getHeuristic(neighborIndexes[i], goal_index)+cost_potential[neighborIndexes[i]];    //A* Algorithm
                    cameFrom[neighborIndexes[i]] = currentNode.index;
                    priority_costs.insert(nextNode);
                }
            }
        }
        
        if(cameFrom[goal_index] == -1){
            cout << "Goal not reachable, failed making a global path." << endl;
            return false;
        }
        
        if(start_index == goal_index)
            return false;
        //Finding the best path
        vector<int> bestPath;
        currentNode.index = goal_index;
        while(currentNode.index != start_index){
            bestPath.push_back(cameFrom[currentNode.index]);
            currentNode.index = cameFrom[currentNode.index];
        }
        reverse(bestPath.begin(), bestPath.end());
        
        ros::Time plan_time = ros::Time::now();
        for(int i = 0; i < bestPath.size(); i++){
          unsigned int tmp1, tmp2;
          costmap_->indexToCells(bestPath[i], tmp1, tmp2);
          double x, y;
          costmap_->mapToWorld(tmp1,tmp2, x, y);

          geometry_msgs::PoseStamped pose;
          pose.header.stamp = plan_time;
          pose.header.frame_id = costmap_ros_->getGlobalFrameID();
          pose.pose.position.x = x;
          pose.pose.position.y = y;
          pose.pose.position.z = 0.0;

          pose.pose.orientation.x = 0.0;
          pose.pose.orientation.y = 0.0;
          pose.pose.orientation.z = 0.0;
          pose.pose.orientation.w = 1.0;

          plan.push_back(pose);
        }
        plan.push_back(goal);
        publishPlan(plan);
        

        return true;
       
    }
    
    double straightNav::getMoveCost(unsigned int firstIndex,unsigned int secondIndex)
    {
        unsigned int tmp1, tmp2;
        costmap_->indexToCells(firstIndex, tmp1, tmp2);
        int firstXCord = tmp1,firstYCord = tmp2;
        costmap_->indexToCells(secondIndex, tmp1, tmp2);
        int secondXCord = tmp1, secondYCord = tmp2;
        
        int difference = abs(firstXCord - secondXCord) + abs(firstYCord - secondYCord);
        // Error checking
        if(difference != 1 && difference != 2){
            ROS_ERROR("Astar global planner: Error in getMoveCost - difference not valid");
            return 1.0;
        }
        if(difference == 1)
            return 1.0;
        else
            return 1.5;
    }
    
    double straightNav::getHeuristic(int cell_index, int goal_index)
    {
        unsigned int tmp1, tmp2;
        costmap_->indexToCells(cell_index, tmp1, tmp2);
        int startX = tmp1, startY = tmp2;
        costmap_->indexToCells(goal_index, tmp1, tmp2);
        int goalX = tmp1, goalY = tmp2;
        double dis=0;
        if(abs(goalY-startY)>abs(goalX-startX)){
        	dis=sqrt((goalX-startX)*(goalX-startX))+abs(abs(goalY-startY)-abs(goalX-startX));
        }else{
        	dis=sqrt((goalY-startY)*(goalY-startY))+abs(abs(goalY-startY)-abs(goalX-startX));
        }
        return dis;
    }
    
    bool straightNav::isInBounds(unsigned int x,unsigned int y)
    {
        if( x < 0 || y < 0 || x >= width || y >= height)
            return false;
        return true;
    }
    
    vector<unsigned int> straightNav::get_neighbors(unsigned int current_cell)
    {   
        vector<unsigned int> neighborIndexes;
        
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                unsigned tmp1, tmp2;
                costmap_->indexToCells(current_cell, tmp1, tmp2);
                int nextX = tmp1 + i;
                int nextY = tmp2 + j;
                int nextIndex = costmap_->getIndex(nextX, nextY);
                if(!( i == 0 && j == 0) && isInBounds(nextX, nextY) && OGM[nextIndex])
                {
                    neighborIndexes.push_back(nextIndex);
                }
            }
        }
        return neighborIndexes;
    }
    
    // double straightNav::mul_mul(double k,double m){
    //     double sum=k;
    //     if(m==0){
    //         return 1;
    //     }
    //     for(int i=1;i<m;i++){
    //         sum=sum*k;
    //     }
    //     return sum;
    // }
    
    // int straightNav::C_(int n,int i){
    //     double sum=0,sum_1=1,sum_2=1,sum_3=1;
    //     if(i==0){
    //         return 1;
    //     }
    //     for(int j=1;j<=n;j++){
    //         sum_1=sum_1*j;//n!
    //     }
    //     for(int j=1;j<=i;j++){
    //         sum_2=sum_2*j;//i!
    //     }
    //     for(int j=1;j<=(n-i);j++){
    //         sum_3=sum_3*j;//(n-i)!
    //     }
    //     sum=sum_1/sum_2/sum_3;
    //     return sum;
    // }

    void straightNav::publishPlan(const std::vector<geometry_msgs::PoseStamped>& path) {
        if (!initialized_) {
            ROS_ERROR(
                    "This planner has not been initialized yet, but it is being used, please call initialize() before use");
            return;
        }

        nav_msgs::Path gui_path;
        gui_path.poses.resize(path.size());

        gui_path.header.frame_id = frame_id_;
        gui_path.header.stamp = ros::Time::now();

        for (unsigned int i = 0; i < path.size(); i++) {
            gui_path.poses[i] = path[i];
        }

        

        plan_original.publish(gui_path);
        // plan_pub_.publish(new_plan);
    }

    // int straightNav::binomialCoefficient(int n, int k) {
    //     if (k == 0 || k == n)
    //         return 1;
    //     else
    //         return binomialCoefficient(n - 1, k - 1) + binomialCoefficient(n - 1, k);
    // }

    // void straightNav::circle(unsigned int current_cell){
    //     for (int i = -20; i <= 20; i++)
    //     {
    //         for (int j = -20; j <= 20; j++)
    //         {
    //             unsigned tmp1, tmp2;
    //             costmap_->indexToCells(current_cell, tmp1, tmp2);
    //             int nextX = tmp1 + i;
    //             int nextY = tmp2 + j;
    //             double cost_sum=(40-abs(i)-abs(j))*3;
    //             if(isInBounds(nextX,nextY)){
    //                 int nextIndex = costmap_->getIndex(nextX, nextY);
    //                 if(cost_potential[nextIndex]==0){
    //                     cost_potential[nextIndex]=cost_sum;
    //                 }else{
    //                     if(cost_potential[nextIndex]<cost_sum){
    //                         cost_potential[nextIndex]=cost_sum;
    //                     }
    //                 }
                    
    //             }
    //         }
    //     }
    // }
};



bool operator <(const Node& x, const Node& y) {
  return x.cost < y.cost;
}
