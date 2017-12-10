
# WebIoPi Setup
----------------

## Raspberry Pi

Marvin's core resides on a Raspberry Pi. A lightweight, headless distro is used, but can be substituted for nearly any other flavor.

### Raspbian Lite
* Download [Raspian Lite](https://www.raspberrypi.org/downloads/raspbian)
* [Install the distro to an SD Card](https://www.raspberrypi.org/documentation/installation/installing-images)

### WebIoPi
* [Download install files](http://webiopi.trouch.com/INSTALL.html)
```
wget http://sourceforge.net/projects/webiopi/files/WebIOPi-0.7.1.tar.gz
tar xvzf WebIOPi-0.7.1.tar.gz
cd WebIOPi-0.7.1
```
* Patch the install file (if using Pi B+/2/3 with 40 pin header):
```
wget https://raw.githubusercontent.com/doublebind/raspi/master/webiopi-pi2bplus.patch
patch -p1 -i webiopi-pi2bplus.patch
```
* Run the setup script:
```
sudo ./setup.sh
```

### InfluxDb
* Install [InfluxDb](https://influxdata.com/downloads/#influxdb)
```
wget https://dl.influxdata.com/influxdb/releases/influxdb_0.13.0_armhf.deb
sudo dpkg -i influxdb_0.13.0_armhf.deb
```
* Edit Influx configuration (if desired):
```
	sudo nano /etc/influxdb/influxdb.conf
```
* Start Influx service:
```
	sudo service influxdb start
```

### Telegraf
* Install [Telegraf](https://influxdata.com/downloads/#telegraf)
```
wget https://dl.influxdata.com/telegraf/releases/telegraf_0.13.1_armhf.deb
sudo dpkg -i telegraf_0.13.1_armhf.deb
```
* Edit Telegraf configuration (if desired):
```
sudo nano /etc/telegraf/telegraf.conf
```
* Start Telegraf service:
```
sudo service telegraf start
```

### MQTT
* Install [Mosquitto MQTT](http://mosquitto.org/) broker and client
```
sudo apt-get install mosquitto mosquitto-clients
```
* Enable MQTT input in telegraf
```
sudo /etc/telegraf/telegraf.conf
** MQTT bits are towards the end of the file **
```
