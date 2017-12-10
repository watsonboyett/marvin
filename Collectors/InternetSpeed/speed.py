# ----- Dependencies -----
# This script uses the speedtest-cli python module to gather internet speed data.
#   To install: "pip install speedtest-cli" or "easy_install speedtest-cli"
#
# This script uses the paho-mqtt python module to publish results to a telegraf server. 
#   To install: "pip install paho-mqtt" 
#
# ----- Scheduling -----
# This script can run on a repeated schedule using crontab (for linux).
#   To run this script every 20 minutes using crontab:
#        * run "sudo crontab -e"
#        * insert "*/20 * * * * cd folder/containing/this/script && /usr/bin/python this_script.py" at the bottom of the file
#        * it may be necessary to run "chmod a+x this_script.py" for this file
#        * since cron doesn't import all environment paths, you may need to point subprocess.Popen to /usr/local/bin/speedtest

import subprocess as sp
import time
import paho.mqtt.publish as publish


RUN_SPEEDTEST = True
PUBLISH_RESULTS = True

file_name = "speed.txt"


# run the speedtest-cli test and save results to a file
success = False
time_start = time.time()
if RUN_SPEEDTEST:
    try:
        with open(file_name, "w+") as file:
            ret_code = sp.Popen(["speedtest", "--simple"], stdout=file).wait()
            success = ret_code == 0
    except:
        print("Failed to complete speedtest.")
        success = False


ping = 0
download = 0
upload = 0

if success:
    # parse the results file
    with open(file_name, "r+") as file:
        for line in file:
            tokens = line.rstrip().split(":")
            subtokens = tokens[1].lstrip().split(" ")

            metric = tokens[0]
            value = subtokens[0]
            units = subtokens[1]

            if metric == "Ping":
                ping = float(value)
            elif metric == "Download":
                download = float(value)
            elif metric == "Upload":
                upload = float(value)

time_end = time.time()
time_elapsed = (time_end - time_start)

print("Ping (ms): {0:0.2f}".format(ping))
print("Download (Mbps): {0:0.2f}".format(download))
print("Upload (Mbps): {0:0.2f}".format(upload))
print("Time (s): {0:0.3f}".format(time_elapsed))


# publish the results to a telegraf server
if PUBLISH_RESULTS:
    mqtt_host = "nuc.local"
    mqtt_topic = "internet"
    mqtt_msg = "internet_speed ping_ms={0:0.2f},download_Mbps={1:0.2f},upload_Mbps={2:0.2f},time_s={3:0.3f}".format(ping, download, upload, time_elapsed)
    publish.single(mqtt_topic, mqtt_msg, hostname=mqtt_host)

