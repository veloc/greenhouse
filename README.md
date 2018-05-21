# greenhouse

## Aim:

Create a setup to monitor multiple points in regards to humidity and temperature.
The system should:
- work without an internet connection (but should also be able to be connected to it)
- have sensor-modules that use an independent power source (battery, solar powered)
- have water proof or "ruggedized" sensor-modules
- should be scalable as in how many sensor-modules can be connected
- present the values visualized as graphs
- create alarms (audible / visual / electronical) if certain criteras are met
- be easy to use

	
## Realization (at the moment):
A raspberryPi with a 5" touchscreen running Raspbian with an SQL-Server, webserver and WLAN accesspoint receives
data from ESP32-Modules via MQTT-Messages over WLAN. Grafana is used to create a graphical interpretation of the
collected data and presents it as a website, accessible from other devices as well.

The ESP32-Modules are connected to a BME280 sensor for air pressure, temperature and humidity.

## Task list (updated now and then)
- [ ] Write a documentation for the Pi-Part
- [ ] Test the "Giesomat"-Sensors
- [ ] Determine the needed powersupply to run the Sensor for as long as possible (a couple of months would be nice...)
- [ ] Sort through the .ino code and make it more easy to read and to understand 
- [ ] Design (and print) a case for the Receiver
- [ ] Design (and print) a case for the Sensor
- [ ] Bughunting!

## Partlist (used or intended to use):

1. Microcontroller
   - Raspberry Pi 3 Model B		https://www.amazon.de/gp/product/B01CD5VC92/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1
     - Elecrow 5" Touch LCD 	https://www.amazon.de/gp/product/B013JECYF2/ref=oh_aui_detailpage_o08_s00?ie=UTF8&psc=1
   - ESP32 NodeMCU				https://www.amazon.de/gp/product/B074RG86SR/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1
2. Sensors
   - CQRobot Soil Sensor		https://www.amazon.de/gp/product/B01E8M3JP8/ref=oh_aui_detailpage_o08_s00?ie=UTF8&psc=1
   - GY-BME80 Air Sensor 		https://www.amazon.de/gp/product/B077PNKCQ6/ref=oh_aui_detailpage_o08_s00?ie=UTF8&psc=1
   - Giesomat Sensor Rev.2		https://www.ramser-elektro.at/shop/bausaetze-und-platinen/giesomat-kapazitiver-bodenfeuchtesensor-erdfeuchtesensor-mit-beschichtung/
3. Power Supply
   - Xoro MPB 250 Powerbank		https://www.amazon.de/gp/product/B00KM44L72/ref=oh_aui_search_detailpage?ie=UTF8&psc=1
   - USB Multimeter				https://www.amazon.de/gp/product/B01DIPF350/ref=oh_aui_search_detailpage?ie=UTF8&psc=1
   - 3x AAA Battery Holder		https://www.amazon.de/gp/product/B01GLOLIX6/ref=oh_aui_detailpage_o04_s00?ie=UTF8&psc=1
   - AAA 2400mAh NiMH Akku		https://www.amazon.de/gp/product/B002HFL5AU/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1
   
## Credits
- I am using lzkelley´s bkup_rpimage-script from https://github.com/lzkelley/bkup_rpimage
- My code originally based on the ESP32 Weather Station Project v1.00 from http://educ8s.tv/esp32-weather-station
- Some of my code is the work of some brilliant people from the internet I just copied and forgot to document - so, if you see your code here, this is directed to YOU! Thanks!

## Log:

### 2018-05-20	Energy Issues & Bugs

I´ve let the Sensor run on battery till it was empty. I am not 100% sure I charged the battery to the maximum
but it ran for about 102 hours (and 30 minutes). Since I´ve used a 2500mAh battery, this indicates the device
uses somewhere around 25 mA (or I´ve miscalculated).

The "USB Safety Tester" https://www.amazon.de/gp/product/B01DIPF350/ref=oh_aui_search_detailpage?ie=UTF8&psc=1
shows strange values (for example, Time not always counts up but stays at a given value, also the "Current Draw"
is listed as 0.00 A if the ESP is in DeepSleep and around 0.16 A if it is running.). With these values, the battery
should have lasted longer. I suspect the LED on the batterypack is one of the reasons, the battery only lasted
~5 days.

I have ordered some other pices to maybe build my own multimeter with a logging function to be able to determine
how much current is being drawn when which action is running.

I also ordered another type of batteries (NiMH) and a holder for these. I am a little concerned, the batterypack
was I originally using might be hazardous.

Since I noticed a couple of odd or unwanted behaviour in my setup, I´ve started a file containing the bugs I found
(and maybe what I did to remove them eventually...).

The Bugs should be found in the **bugs.md** file.

I also modified this file (a lot) with additional content and formatting.


### 2018-05-14	Thoughts about OTA-Updates - do I need these right now?

At the moment, I am playing around with the idea of updating the ESP32-Modules via OTA but as they do sleep most
of the time and one of the goals is to last as long as possible on a battery, I find it difficult to waste 15s
just checking if an OTA-Update is being pushed.
I think I will let the OTA-Update idea rest for a while and concentrate on other parts of the project.