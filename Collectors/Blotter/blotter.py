#
# https://data.nola.gov/Public-Safety-and-Preparedness/Calls-for-Service-2017/bqmt-f3jk
#

import base64
import json
from datetime import datetime, timedelta
from time import gmtime, strftime
import urllib.parse
import urllib.request
import os.path
import gmplot

fname = 'results.json'

if not os.path.isfile(fname):
    print("Getting data from web API: ")

    date_today = datetime.now()
    date_today_str = date_today.strftime('\"%Y-%m-%d\"')
    date_yesterday = date_today - timedelta(days=1)
    date_yesterday_str = date_yesterday.strftime('\"%Y-%m-%d\"')

    url_base = "https://data.nola.gov/resource/bevf-jgit.json?"
    url_query = "$where=within_circle(location,29.972343,-90.072902,700)" + \
                " and timecreate>=" + date_yesterday_str + \
                " and timecreate<" + date_today_str

    url = url_base + urllib.parse.quote_plus(url_query, safe='/=><')
    print(url)

    response = urllib.request.urlopen(url)
    data = json.loads(response.read().decode('utf-8'))

    with open(fname, 'w') as outfile:
        json.dump(data, outfile)

else:
    print("Loading data from file")
    with open(fname, 'r') as infile:
        data = json.load(infile)

print(data)    

marker_lats = [rec['location']['coordinates'][1] for rec in data]
marker_lngs = [rec['location']['coordinates'][0] for rec in data]

print(marker_lats)

gmap = gmplot.GoogleMapPlotter(29.9712646, -90.0688038, 15)
gmap.scatter(marker_lats, marker_lngs, 'k', marker=False, size=20)
gmap.draw("map.html")

