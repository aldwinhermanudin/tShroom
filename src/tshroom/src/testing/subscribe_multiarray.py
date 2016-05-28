#!/usr/bin/env python

import rospy
import time
import random
import datetime
import telepot
from std_msgs.msg import String
from std_msgs.msg import Float64MultiArray

global tshroom_sensors
tshroom_sensors = []

def callback(data):
    rospy.loginfo("tShroom Message :  %s",data.data)
    print

def sensorsCB(msg):
    global tshroom_sensors
    del tshroom_sensors[:]
    for sensors in msg.data:
        tshroom_sensors.append(sensors)

    for sensors in tshroom_sensors:
        rospy.loginfo("tShroom Message :  %s",sensors)

rospy.init_node('telegram_api')
rospy.Subscriber("/tshroom/communication/telegram", String, callback)
rospy.Subscriber("/tshroom/sensors", Float64MultiArray, sensorsCB);

rospy.spin()
