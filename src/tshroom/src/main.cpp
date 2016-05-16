#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include <iostream>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Int32MultiArray.h"
#include "std_msgs/Float64MultiArray.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Float64.h"

#define STEERING 0
#define THROTTLE 2

void sensorsCB(const std_msgs::Float64MultiArray& msg);
void blobSizeCB(const std_msgs::Float64& msg);
void activePointCB(const std_msgs::Int32& msg);

double blob_size	= 0 ;
int active_point	= 0 ;
double humidity		= 0 ;
double temperature	= 0 ;
double pressure		= 0 ;

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

void sensorsCB(const std_msgs::Float64MultiArray& msg){
	
	humidity		= msg.data[0] ;
	temperature		= msg.data[1] ;
	pressure		= msg.data[2] ;
	
}
void blobSizeCB(const std_msgs::Float64& msg){
	
	blob_size = msg.data;
}
void activePointCB(const std_msgs::Int32& msg){
	
	active_point = msg.data;
}
