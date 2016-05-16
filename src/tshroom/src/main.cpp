#include "ros/ros.h"
#include "mavros_msgs/OverrideRCIn.h"
#include "auvsi16/overrideMotorRC.h"
#include "std_msgs/Bool.h"
#include <iostream>

#define STEERING 0
#define THROTTLE 2

void overrideInputCB(const auvsi16::overrideMotorRC& override_recv);
void overrideStatusCB(const std_msgs::Bool& override_status_recv);
bool last_override_status = true;

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

void overrideStatusCB(const std_msgs::Bool& override_status_recv){
	
	override_status = override_status_recv.data;
	
}

void overrideInputCB(const auvsi16::overrideMotorRC& override_recv){
	mavros_msgs::OverrideRCIn override_out;
  // Drone control 
  if(override_status){
	for(int i=0; i < 8; i++) override_out.channels[i] = 0;	//Releases all Channels First
	
	if (override_recv.throttle > 1942){
		override_out.channels[THROTTLE] = 1942;
	} 	
	else if (override_recv.throttle < 1101){
		override_out.channels[THROTTLE] = 1101;
	}	
	else {
		override_out.channels[THROTTLE] = override_recv.throttle;
	}
	
	if (override_recv.steering > 1942){
		override_out.channels[STEERING] = 1942;
	}	
	else if (override_recv.steering < 1116){
		override_out.channels[STEERING] = 1116;
	}	
	else {
		override_out.channels[STEERING] = override_recv.steering;
  	}
  
      pub_override_rc.publish(override_out);
      
	last_override_status = override_status;
 }
  
  // RC take control
  else if(!override_status && last_override_status){
	
	ROS_WARN_STREAM( "[MC] Override Off") ;
	last_override_status = override_status;
  }
}
