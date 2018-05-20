# List of known bugs

BugID is different for 
- Bugs that concern the Pi (receiver) indicated by the BugID "BREC00000"
- Bugs that concern the ESP (sensor) indicated by the BugID "BSEN00000"
- and general bugs indicated by the BugID "BGEN00000"

##Done? / BugID / Bug Description
- [ ] / BREC00001 / mqtt2db_runner.sh startup via crontab entry "@reboot" does not seem to work
- [ ] / BREC00002 / mqtt2dt.py inserts old values into the DB when it is being started
- [ ] / BSEN00001 / sometimes the ESP only delivers a Air Humidity value.
        - 2018-05-20: maybe 60ms is not enough "waitForMQTT" to transmit every data? Ive increased it to 80ms.