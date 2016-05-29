#include "ros/ros.h"
#include <string>     // std::string, std::to_string#include <iostream>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Int32MultiArray.h"
#include "std_msgs/Float64MultiArray.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Float64.h"
#include "std_msgs/String.h"

void sensorsCB(const std_msgs::Float64MultiArray& msg);
void blobSizeCB(const std_msgs::Float64& msg);
void activePointCB(const std_msgs::Int32& msg);

double  FINISHED_BLOB_RATIO = 3 ;
double  HUMIDITY_THRES      = 50 ;
double  TEMPERATURE_THRES   = 60 ;
double  blob_size	          = 0 ;
int     active_point        = 0 ;
double  humidity		        = 0 ;
double  temperature	        = 0 ;
double  pressure		        = 0 ;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "main_tshroom");
  ros::NodeHandle nh("~");
  nh.getParam("finished_blob_ratio", FINISHED_BLOB_RATIO);
	nh.getParam("humidity_thres", HUMIDITY_THRES);
	nh.getParam("temperature_thres", TEMPERATURE_THRES);

  ros::Subscriber sub_sensors               = nh.subscribe("/tshroom/sensors", 1, sensorsCB);
  ros::Subscriber sub_blob_size             = nh.subscribe("/tshroom/cv/size", 1, blobSizeCB);
  ros::Subscriber sub_active_point          = nh.subscribe("/tshroom/cv/active_point", 1, activePointCB);
  ros::Publisher pub_pump_control           = nh.advertise<std_msgs::Int32>("/tshroom/output/pump", 10,1);
  ros::Publisher pub_communication_telegram = nh.advertise<std_msgs::String>("/tshroom/communication/telegram", 10,1);
  ROS_INFO_STREAM("Starting Main Mushroom Program");
  ROS_INFO_STREAM("Wait for 10 Seconds");
  sleep(10);

  while(ros::ok() && (blob_size == 0 || humidity == 0 ||	temperature	== 0 || pressure == 0)){
    ros::spinOnce();
  }

  double initial_blob = blob_size;
  std_msgs::String telegram_message;
  bool pump_flag = true;
  std_msgs::Int32 pump_state;

  // Set default value for pump state
  pump_state.data = 0;
  pub_pump_control.publish(pump_state);

  telegram_message.data = "Starting mushroom program with blob size of  ";
  std::ostringstream strs;
  strs << blob_size;
  std::string str = strs.str();
  telegram_message.data += str;
  pub_communication_telegram.publish(telegram_message);
  ROS_INFO_STREAM(telegram_message);

  while(ros::ok()){
    ros::spinOnce();
    double current_size_ratio = blob_size/initial_blob;
    ROS_INFO_STREAM(current_size_ratio);
    if (current_size_ratio > FINISHED_BLOB_RATIO ){
      telegram_message.data = "Your mushroom is done with blob size of ";
      std::ostringstream strs;
      strs << blob_size;
      std::string str = strs.str();
      telegram_message.data += str;
      pub_communication_telegram.publish(telegram_message);
      pump_state.data = 0;
      pub_pump_control.publish(pump_state);
      sleep(10);
      break;
    }

    if (HUMIDITY_THRES > humidity && pump_flag){
      pump_state.data = 1;
      pub_pump_control.publish(pump_state);
      pump_flag = false;
    }

    else if (HUMIDITY_THRES < humidity && !pump_flag) {
      pump_state.data = 0;
      pub_pump_control.publish(pump_state);
      pump_flag = true;
    }
  }

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
