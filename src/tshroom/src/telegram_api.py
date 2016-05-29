#!/usr/bin/env python

import rospy
import time
import random
import datetime
import telepot
from std_msgs.msg import String
from std_msgs.msg import Float64MultiArray

channel_name = "@tshroom_device"
bot_token = "235940050:AAGL-96zDkIkL9DXUhvYs_38_G06JEC0XkE"
owner_id = "139937108"

global tshroom_sensors
global telegram_message
tshroom_sensors = [0,0,0]
telegram_message = "Temporary Message"

def communicationCB(msg):
    bot.sendMessage(channel_name, msg.data)
    bot.sendMessage(owner_id, msg.data)
    rospy.loginfo(msg.data)

def sensorsCB(msg):
    global tshroom_sensors
    del tshroom_sensors[:]
    for sensors in msg.data:
        tshroom_sensors.append(sensors)

def handle(msg):
    chat_id = msg['chat']['id']
    command = msg['text']

    print 'Got command: %s' % command

    if command == '/time':
        bot.sendMessage(chat_id, str(datetime.datetime.now()))
    elif command == '/status':
        global tshroom_sensors
        telegram_status_message = "tShroom Sensor Data \n\nHumidity    : %.2f percent\nTemperature : %.2f celcius \nPressure    : %.2f pascal" % (tshroom_sensors[0],tshroom_sensors[1],tshroom_sensors[2])
        bot.sendMessage(chat_id, telegram_status_message)


bot = telepot.Bot(bot_token)
bot.message_loop(handle)

rospy.init_node('telegram_api')
rospy.Subscriber("/tshroom/communication/telegram", String, communicationCB)
rospy.Subscriber("/tshroom/sensors", Float64MultiArray, sensorsCB);
rospy.spin()
