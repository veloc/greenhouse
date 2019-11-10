# Documentation on how to setup the Pi
##	I will line out how I´ve set up my Raspberry Pi to
## 	- open a wireless network for the Sensors to connect to
##	- run the service that receives the data from the sensors
## 	- host a website that shows the collected data

### Hardware:
   - Raspberry Pi 3 Model B		https://www.amazon.de/gp/product/B01CD5VC92/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1
     - Elecrow 5" Touch LCD 	https://www.amazon.de/gp/product/B013JECYF2/ref=oh_aui_detailpage_o08_s00?ie=UTF8&psc=1
	 - 32 GB microSD Card (only 8 GB are needed for the image but I had no smaller laying arround...)

### Logical Connection Scheme:

PC --> LAN --> Website
				  ^
				  |
				  Pi
				  ^
				  |
			   Database <-- WLAN <-- ESP32
			   
### Software

- Raspbian Image from here:
		https://www.waveshare.com/wiki/5inch_HDMI_LCD#Software
	I used "5inch_raspbian20170705.img".
	This image contains Raspbian 8.
- I am using lzkelley´s bkup_rpimage-script from https://github.com/lzkelley/bkup_rpimage
	
### Post OS deployment

#### Updates
- connect to web
	--> sudo apt update
	--> sudo apt upgrade
	
#### Additional Software
- I like editing with vim, so I install vim...
- screen is always needed
- for easy access to the storage for the backup, I install and configure autofs. hardmounted mountpoints would also work, i assume...
- pv is needed by the backup script

sudo apt install vim screen autofs pv

##### autofs
sudo vim /etc/auto.nfs
	Add the line:
		Backup          -fstype=nfs,rw,retry=0 192.168.178.4:/Backup

sudo vim /etc/auto.master
	Add the line:
		/mnt    /etc/auto.nfs

sudo service autofs restart
###### dont forget to set the permissions serverwise!

##### bkup_rpimage-script
I´ve written a little start script to be able to just fire and forget the bkup_rpimage-script.
My forked version can be found at https://github.com/veloc/bkup_rpimage
both scripts are being made executable and are being put at /usr/bin:

sudo chmod +x /usr/bin/run_bkup.sh
sudo chmod +x /usr/bin/bkup_rpimage.sh

the script now puts a complete backup of the whole sd card onto my NAS.

### Security & Cosmetics
sudo raspi-config
	--> change User Password
	--> change Hostname to "greenhouse" 