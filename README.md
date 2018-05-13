# greenhouse

Aim:
create a setup to monitor multiple points in regards to humidity and temperature.
Values should be visualized in graphs.

Realization (at the moment):
A raspberryPi with a 5" touchscreen running Raspbian with an SQL-Server, webserver and WLAN accesspoint receives
data from ESP32-Modules via MQTT-Messages over WLAN. Grafana is used to create a graphical interpretation of the
collected data and presents it as a website, accessible from other devices as well.

The ESP32-Modules are connected to a BME280 sensor for air pressure, temperature and humidity.