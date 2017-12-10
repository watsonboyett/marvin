# weather forecast: https://graphical.weather.gov/xml/sample_products/browser_interface/ndfdXMLclient.php?zipCodeList=70116&product=time-series&maxt=maxt&mint=mint&temp=temp&dew=dew&pop12=pop12&sky=sky&rh=rh&wspd=wspd&wdir=wdir&wx=wx
# current weather: http://w1.weather.gov/xml/current_obs/KNBG.xml


import urllib.request
from collections import namedtuple
from bs4 import BeautifulSoup
from datetime import datetime
import pytz
from influxdb import InfluxDBClient

stations = ['KNBG', 'KMSY', 'KNEW']


class WeatherCurrent:
    Measures = namedtuple('Measures', 'station, time_utc, weather, ' +
        'temperature_f, heat_index_f, dewpoint_f, relative_humidity, ' +
        'wind_deg, wind_mph, wind_dir, visibility_mi, pressure_mb')

    def __init__(self):
        self.base_url = 'http://w1.weather.gov/xml/current_obs/'

    def parse_station_xml(self, xml):
        tags = BeautifulSoup(xml, features="xml")

        # parse time into UTC format (needed by influx)
        time_str = tags.observation_time_rfc822.string
        time = datetime.strptime(time_str, '%a, %d %b %Y %H:%M:%S %z')
        time_utc = time.astimezone(pytz.utc).strftime("%Y-%m-%dT%H:%M:%SZ")

        get_value = lambda x : x.string if x is not None else '0'

        # store as tuple (not strictly necessary here)
        data = WeatherCurrent.Measures(
            station = get_value(tags.station_id), 
            time_utc = time_utc, 
            weather = get_value(tags.weather),
            temperature_f = float(get_value(tags.temp_f)), 
            heat_index_f = float(get_value(tags.heat_index_f)), 
            dewpoint_f = float(get_value(tags.dewpoint_f)), 
            relative_humidity = float(get_value(tags.relative_humidity)),
            wind_deg = float(get_value(tags.wind_degrees)), 
            wind_mph = float(get_value(tags.wind_mph)), 
            wind_dir = get_value(tags.wind_dir),
            visibility_mi = float(get_value(tags.visibility_mi)), 
            pressure_mb = float(get_value(tags.pressure_mb))
        )

        return data

    def get_station_data(self, station):

        # get data from NWS XML stream
        url = self.base_url + station + '.xml'
        response = urllib.request.urlopen(url)
        data = self.parse_station_xml(response)
        return data


class InfluxWriter:

    def __init__(self, dbname, host='localhost', port=8086):
        self.client = InfluxDBClient(host=host, port=port, database=dbname)

    # save data (in json format) to influx database
    def write_data(self, json):
        self.client.write_points(json)


if __name__ == '__main__':

    wc = WeatherCurrent()
    iw = InfluxWriter(host='192.168.0.10', dbname='weather')

    for station in stations:
        data = wc.get_station_data(station)
        print(data)

        json = [
            {
                "measurement": "current",
                "tags": {
                    "station": data.station
                },
                "time": data.time_utc,
                "fields": {
                    "weather": data.weather,
                    "temperature_f": data.temperature_f,
                    "heat_index_f": data.heat_index_f,
                    "dewpoint_f": data.dewpoint_f,
                    "relative_humidity": data.relative_humidity,
                    "wind_deg": data.wind_deg,
                    "wind_mph": data.wind_mph,
                    "wind_dir": data.wind_dir,
                    "visibility_mi": data.visibility_mi,
                    "pressure_mb": data.pressure_mb,
                }
            }
        ]

        iw.write_data(json)
    
