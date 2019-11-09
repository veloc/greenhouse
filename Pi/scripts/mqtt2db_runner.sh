#! /bin/bash

# taken from:
#  https://stackoverflow.com/questions/20162678/linux-script-to-check-if-process-is-running-act-on-the-result

LOGDIR=/var/log/mqtt
LOGFILE=$LOGDIR/mqtt2db.log

if [ ! -d "$LOGFILE" ]; then

	if [ ! -d "$LOGDIR" ]; then
		echo "LOGDIR not existant, creating $LOGDIR"
		mkdir -p -- "$LOGDIR"
	fi
	
	echo "touching $LOGFILE..."
		touch $LOGFILE || exit
else
	echo "LOGFILE $LOGFILE was there..."
fi

case "$(pidof python mqtt2db.py | wc -w)" in

0)	echo "mqtt2db is not running, starting it:	$(date)" >> $LOGFILE
	/usr/bin/python /home/pi/scripts/mqtt2db.py &
	;;

esac
