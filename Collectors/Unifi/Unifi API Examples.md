
## Some Open-Source References

https://github.com/Art-of-WiFi/UniFi-API-client/blob/master/src/Client.php
https://github.com/calmh/unifi-api/blob/master/unifi/controller.py


## Some cUrl Examples

Authenticate your session and store the returned cookie:
```
curl 'https://controller_ip:port/api/login' --data-binary '{"username":"user","password":"pass","strict":true}' --compressed --insecure -c cookies.txt
```

Use the stored cookie to send a status request to the controller:
```
curl --insecure -b cookies.txt -c cookies.txt 'https://controller_ip:port/api/s/default/stat/sta' > results.json  
```


## Some Other API Destinations
```
https://controller_ip:port/api/s/default/stat/health

https://controller_ip:port/api/s/default/stat/event?within=24&_limit=10
https://controller_ip:port/api/s/default/stat/alarm?within=24

https://controller_ip:port/api/s/default/list/alarm
https://controller_ip:port/api/s/default/list/event
```

