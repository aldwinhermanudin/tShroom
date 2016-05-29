/*
 * rosserial PubSub Example
 * Prints "hello world!" and toggles led
 */

#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Float64MultiArray.h>

ros::NodeHandle  nh;


void messageCb( const std_msgs::Int32& toggle_msg){
  digitalWrite(13, HIGH-digitalRead(13));   // blink the led
}

ros::Subscriber<std_msgs::Int32> sub("toggle_led", messageCb );

std_msgs::Float64MultiArray sensor_msg;
ros::Publisher pub_sensor("/tshroom/sensors", &sensor_msg);

void setup()
{
  pinMode(13, OUTPUT);
  nh.initNode();
  nh.advertise(pub_sensor);
  nh.subscribe(sub);
}

void loop()
{
  float sensor_msg_buff[3];
  sensor_msg_buff[0] = 1;
  sensor_msg_buff[1] = 2;
  sensor_msg_buff[2] = 3;
  
  sensor_msg.data = sensor_msg_buff;
  sensor_msg.data_length = 3;

  //Publish array
  pub_sensor.publish( &sensor_msg );
  nh.spinOnce();
  delay(3000);
}

