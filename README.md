# greenhouse

Aim:
Create a setup to monitor multiple points in regards to humidity and temperature.
The system should
	- work without an internet connection (but should also be able to be connected to it)
	- have sensor-modules that use an independent power source (battery, solar powered)
	- have water proof or "ruggedized" sensor-modules
	- should be scalable as in how many sensor-modules can be connected
	- present the values visualized as graphs
	- create alarms (audible / visual / electronical) if certain criteras are met
	- be easy to use

Realization (at the moment):
A raspberryPi with a 5" touchscreen running Raspbian with an SQL-Server, webserver and WLAN accesspoint receives
data from ESP32-Modules via MQTT-Messages over WLAN. Grafana is used to create a graphical interpretation of the
collected data and presents it as a website, accessible from other devices as well.

The ESP32-Modules are connected to a BME280 sensor for air pressure, temperature and humidity.

2018-05-14	Thoughts about OTA-Updates - do I need these right now?
At the moment, I am playing around with the idea of updating the ESP32-Modules via OTA but as they do sleep most
of the time and one of the goals is to last as long as possible on a battery, I find it difficult to waste 15s
just checking if an OTA-Update is being pushed.
I think I will let the OTA-Update idea rest for a while and concentrate on other parts of the project.