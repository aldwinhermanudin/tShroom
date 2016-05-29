#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <ros.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Float64MultiArray.h>

#define DHTPIN 22
#define PUMPPIN 23
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;
ros::NodeHandle  nh;
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 2000;           // interval at which to blink (milliseconds)

void pumpCB( const std_msgs::Int32& msg){
  if (msg.data == 1) {
    digitalWrite(PUMPPIN, HIGH);
  }
  else {
    digitalWrite(PUMPPIN, LOW);
  }
}

ros::Subscriber<std_msgs::Int32> sub_pump("/tshroom/output/pump", pumpCB );
std_msgs::Float64MultiArray sensor_msg;
ros::Publisher pub_sensor("/tshroom/sensors", &sensor_msg);

void setup() {
  nh.initNode();
  nh.advertise(pub_sensor);
  nh.subscribe(sub_pump);

  delay(100);
  pinMode(PUMPPIN, OUTPUT);
  bmp.begin();
  dht.begin();
}

float humid, temp, pressure;

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    humid = dht.readHumidity();
    temp = (dht.readTemperature() + bmp.readTemperature()) / 2;
    pressure = bmp.readPressure();

    // Check if any reads failed.
    if (isnan(humid)) {
      humid = -1;
    }
    if (isnan(temp)) {
      temp = -1;
    }
    if (isnan(pressure)) {
      pressure = -1;
    }
    float sensor_msg_buff[3];
    sensor_msg_buff[0] = humid;
    sensor_msg_buff[1] = temp;
    sensor_msg_buff[2] = pressure;

    sensor_msg.data = sensor_msg_buff;
    sensor_msg.data_length = 3;

    //Publish array
    pub_sensor.publish( &sensor_msg );
  }

  nh.spinOnce();
  delay(50);
}




