import webiopi
import datetime
import time
import json

# ----------

BUTTON_PUSH_TIME = 2

class RemoteDevice:

    def __init__(self, channel, is_on, pin_on, pin_off):
        self.channel = channel
        self.is_on = is_on
        self.pin_on = pin_on
        self.pin_off = pin_off

    def turn_on(self):
        GPIO.digitalWrite(self.pin_on, GPIO.HIGH)
        time.sleep(BUTTON_PUSH_TIME)
        GPIO.digitalWrite(self.pin_on, GPIO.LOW)
        self.is_on = True

    def turn_off(self):
        GPIO.digitalWrite(self.pin_off, GPIO.HIGH)
        time.sleep(BUTTON_PUSH_TIME)
        GPIO.digitalWrite(self.pin_off, GPIO.LOW)
        self.is_on = False

def json_default(obj):
    return obj.__dict__

# ----------

devices = []
device_channel_map = {}

def get_device_index(channel):
    index = device_channel_map[int(channel)]
    return index

# ----------


@webiopi.macro
def turn_on_device(channel):
    global devices, device_channel_map
    index = get_device_index(channel)
    devices[index].turn_on()

@webiopi.macro
def turn_off_device(channel):
    global devices, device_channel_map
    index = get_device_index(channel)
    devices[index].turn_off()

@webiopi.macro
def is_device_on(channel):
    global devices, device_channel_map
    index = get_device_index(channel)
    return devices[index].is_on

# ----------

DEVICE_CONFIG_FILENAME = "devices.json"

initial_run = True;
if (initial_run):
    ch1 = RemoteDevice(1, False, 17, 27)
    ch2 = RemoteDevice(2, False, 22, 23)
    ch3 = RemoteDevice(3, False, 6, 5)
    ch4 = RemoteDevice(4, False, 13, 12)
    ch5 = RemoteDevice(5, False, 16, 26)
    device_config = [ch1, ch2, ch3, ch4, ch5]

    with open(DEVICE_CONFIG_FILENAME, 'w') as fp:
        json.dump(device_config, fp, default=json_default)

# load device config file
with open(DEVICE_CONFIG_FILENAME, 'r') as fp:
    device_config = json.load(fp)

# initialize device config
index = 0
for device in device_config:
    dev_obj = RemoteDevice(device["channel"], device["is_on"], device["pin_on"], device["pin_off"])
    devices.append(dev_obj)
    device_channel_map[dev_obj.channel] = index
    index += 1

# ----------


LIGHT_ON_TIME  = datetime.time(15, 35)

now = datetime.datetime.now()
LIGHT_OFF_TIME = datetime.time(23, 40)

# ----------

GPIO = webiopi.GPIO

# setup function is automatically called at WebIOPi startup
def setup():
    # configure the GPIO pin direction used by the devices
    for device in devices:
        GPIO.setFunction(device.pin_on, GPIO.OUT)
        GPIO.setFunction(device.pin_off, GPIO.OUT)


# loop function is repeatedly called by WebIOPi 
def loop():
    # retrieve current datetime
    now = datetime.datetime.now()

    # toggle light ON all days at the correct time
    if (now.hour == LIGHT_ON_TIME.hour and now.minute == LIGHT_ON_TIME.minute):
        index = get_device_index(5)
        if (not devices[index].is_on):
            devices[index].turn_on()

    # toggle light OFF
    if (now.hour == LIGHT_OFF_TIME.hour and now.minute == LIGHT_OFF_TIME.minute):
        index = get_device_index(5)
        if (devices[index].is_on):
            devices[index].turn_off()

          

    # gives CPU some time before looping again
    webiopi.sleep(1)

# destroy function is called at WebIOPi shutdown
def destroy():
    for device in devices:
        GPIO.digitalWrite(device.pin_on, GPIO.LOW)
        GPIO.digitalWrite(device.pin_off, GPIO.LOW)

