import time
import random
import datetime
import telepot

"""
A simple bot that accepts two commands:
- /roll : reply with a random integer between 1 and 6, like rolling a dice.
- /time : reply with the current time, like a clock.

INSERT TOKEN below in the code, and run:
$ python diceyclock.py

Ctrl-C to kill.
"""

def handle(msg):
    chat_id = msg['chat']['id']
    command = msg['text']

    print 'Got command: %s' % command

    if command == '/roll':
        bot.sendMessage(chat_id, random.randint(1,6))
        bot.sendMessage('@tshroom_device', random.randint(1,6))
    elif command == '/time':
        bot.sendMessage(chat_id, str(datetime.datetime.now()))
        bot.sendMessage('@tshroom_device', str(datetime.datetime.now()))

bot = telepot.Bot('235940050:AAGL-96zDkIkL9DXUhvYs_38_G06JEC0XkE')
bot.message_loop(handle)
print 'I am listening ...'

while 1:
    time.sleep(10)
