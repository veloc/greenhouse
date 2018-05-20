#! /bin/bash

# taken from:
#  https://stackoverflow.com/questions/20162678/linux-script-to-check-if-process-is-running-act-on-the-result

logfile="/var/log/mqtt/mqtt2db.log"

case "$(pidof python mqtt2db.py | wc -w)" in

0)	echo "mqtt2db is not running, starting it:	$(date)" >> $logfile
	/usr/bin/python /home/pi/scripts/mqtt2db.py &
	;;

esac
