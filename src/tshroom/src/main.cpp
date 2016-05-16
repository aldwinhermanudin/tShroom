#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include <iostream>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Int32MultiArray.h"
#include "std_msgs/Int32.h"

#define STEERING 0
#define THROTTLE 2

void sensorsCB(const std_msgs::Int32MultiArray& msg);
void blobSizeCB(const std_msgs::Int32& msg);
void activePointCB(const std_msgs::Int32& msg);

int main(int argc, char **argv)
{
  ros::init(argc, argv, "main_tshroom");
  ros::NodeHandle nh;

  ros::Subscriber sub_sensors = nh.subscribe("/tshroom/sensors", 1, sensorsCB);
  ros::Subscriber sub_blob_size = nh.subscribe("/tshroom/cv/size", 1, blobSizeCB);
  ros::Subscriber sub_active_point = nh.subscribe("/tshroom/cv/active_point", 1, activePointCB);
  
  ros::spin();
  return 0;
}


