-- rosservice call /finish_trajectory 0
-- rosservice call /write_state "{filename: '${HOME}/slam_map/c-map22-3-2-1804.pbstream'}"
-- rosrun cartographer_ros cartographer_pbstream_to_ros_map -map_filestem=${HOME}/slam_map/c-map22-3-2-1804 -pbstream_filename=${HOME}/slam_map/c-map22-3-2-1804.pbstream -resolution=0.05

include "map_builder.lua"
include "trajectory_builder.lua"

options = {
  map_builder = MAP_BUILDER,
  trajectory_builder = TRAJECTORY_BUILDER,
  map_frame = "map",-- map是世界坐标系，用于发布submap
  -- 若只使用雷达进行建图，则将tracking_frame和published_frame都改为雷达坐标系laser
  -- 若使用雷达和机器人进行建图，不使用轮式里程计，
  -- 则tracking_frame和published_frame都改为base_link，provide_odom_frame改为true
  -- 若使用雷达和机器人进行建图，且使用轮式里程计，
  -- 则tracking_frame改为base_link，published_frame改为odom，provide_odom_frame改为false
  -- 解释一下，odom_frame是cartographer用激光数据计算产生的里程计数据，
  -- provide_odom_frame意思为是否需要cartographer为你提供里程计，注意此处里程计指的是激光里程计数据
  -- 如果将provide_odom_frame设置为false，那么tf关系为map_frame->published_frame，
  -- 如果published_frame此时为base_link，理论上tf关系为map->base_link
  -- 但是由于机器人运行时发布的tf关系为odom->base_link(比如P3)，根据tf的规则一个节点只能有一个父节点，最终会导致报错。
  -- 解决方法就是将published_frame设置为odom.
  tracking_frame = "base_link", --SLAM算法要跟踪和校准的坐标系，如果有imu就应该设置为imu_link
  published_frame = "odom", --一个用来发布姿态的子级坐标系，主要用来校正tracking_frame；
  odom_frame = "odom",  --仅当provide_odom_frame=true时可以使用，位置关系是map_frame->odom_frame->published_frame，被用来发布局部的slam激光里程计结果，不用于回环检测
  provide_odom_frame = false,
  publish_frame_projected_to_2d = true,  --如果为true，则发布的姿态published_frame将会是纯2D姿态，没有横滚，俯仰，偏转角。
  use_pose_extrapolator = true,
  use_odometry = true, --如果为true，将会订阅“odom”这个话题，消息类型是nav_msgs/Odometry。这种情况下，必须提供里程计数据，并且这些数据将会包含进slam
  use_nav_sat = false,  --如果为true，将会订阅“fix”这个话题，消息类型是sensor_msgs/NavSatFix，这种情况下必须提供导航数据，并且这些数据将会包含进全局slam
  use_landmarks = false,  --是否使用landmark
  num_laser_scans = 1,  --订阅单线激光雷达话题的数量
  num_multi_echo_laser_scans = 0, --订阅多线激光雷达话题的数量
  num_subdivisions_per_laser_scan = 10,  --收到的每个多线激光雷达数据分割出的点云的数量。对扫描的数据进行细分使得雷达在移动时获得的数据不会发生畸变，是一种暴力消除激光雷达运动畸变的方法。
  num_point_clouds = 0, --订阅的点云话题的数量
  lookup_transform_timeout_sec = 0.2, --使用tf2查询坐标变换时的超时时间，以秒为单位
  submap_publish_period_sec = 0.3,  --发布子图姿态的时间间隔，以秒为单位
  pose_publish_period_sec = 5e-3, --发布姿态的时间间隔，以秒为单位；5e-3代表5毫秒一次，200hz，一秒200次
  trajectory_publish_period_sec = 30e-3,  --发布轨迹标记的时间间隔，以秒为单位
  rangefinder_sampling_ratio = 1.,  --测距仪的固定采样比例
  odometry_sampling_ratio = 1.0, --里程计的固定采样比例，以下类同
  fixed_frame_pose_sampling_ratio = 1.,
  imu_sampling_ratio = 1.,
  landmarks_sampling_ratio = 1.,
}

MAP_BUILDER.use_trajectory_builder_2d = true  --决定使用2D雷达还是3D雷达
MAP_BUILDER.use_trajectory_builder_3d = false
--TRAJECTORY_BUILDER_2D是前端配置文件
TRAJECTORY_BUILDER_2D.submaps.num_range_data = 30 --submaps插入点云帧的数量
TRAJECTORY_BUILDER_2D.num_accumulated_range_data= 10
TRAJECTORY_BUILDER_2D.min_range = 0.2 --激光雷达测距的最短距离，单位应该是m
TRAJECTORY_BUILDER_2D.max_range = 25  --激光雷达测距的最长距离 。
TRAJECTORY_BUILDER_2D.missing_data_ray_length = 25  --超过测距最长距离的点，将会被插入到这个长度的位置
TRAJECTORY_BUILDER_2D.use_imu_data = false  --是否使用imu数据，3Dslam必选imu数据
TRAJECTORY_BUILDER_2D.use_online_correlative_scan_matching = true --是否先使用相关性扫描匹配(csm)解决在线扫描匹配，从而产生一个好的初始点进行优化
TRAJECTORY_BUILDER_2D.real_time_correlative_scan_matcher.linear_search_window = 0.1 --csm最小的线性搜索窗口，在这个窗口将会找到最佳的扫描队列
TRAJECTORY_BUILDER_2D.real_time_correlative_scan_matcher.translation_delta_cost_weight = 1  --计算帧间匹配得分时，平移和旋转部分数据的权重，权重越高代表越相信该数据
TRAJECTORY_BUILDER_2D.real_time_correlative_scan_matcher.rotation_delta_cost_weight = 1  --同上

--POSE_GRAPH是后端配置文件
POSE_GRAPH.optimization_problem.huber_scale = 1e-2 --残差方程里面的一个参数，数值越大，异常值影响越大
POSE_GRAPH.optimize_every_n_nodes = 60  --每结束一个node都会添加对应的约束。node代表scan match，约束代表相对位姿关系。等待n个node结束后就会进行全局回环检测优化
POSE_GRAPH.constraint_builder.min_score = 0.55  --fast csm的最小分数，高于此分数才会进行优化
POSE_GRAPH.constraint_builder.fast_correlative_scan_matcher.linear_search_window=15
POSE_GRAPH.constraint_builder.fast_correlative_scan_matcher.branch_and_bound_depth=7
POSE_GRAPH.optimization_problem.odometry_translation_weight=0
POSE_GRAPH.optimization_problem.odometry_rotation_weight=0

return options