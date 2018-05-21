# List of known bugs

BugID is different for 
- Bugs that concern the Pi (receiver) indicated by the BugID "BREC00000"
- Bugs that concern the ESP (sensor) indicated by the BugID "BSEN00000"
- and general bugs indicated by the BugID "BGEN00000"

## Done? / BugID / Bug Description
- [ ] / BREC00001 / mqtt2db_runner.sh startup via crontab entry "@reboot" does not seem to work
- [ ] / BREC00002 / mqtt2dt.py inserts old values into the DB when it is being started
- [ ] / BSEN00001 / sometimes the ESP only delivers a Air Humidity value.
        - 2018-05-20_veloc: maybe 60ms is not enough "waitForMQTT" to transmit every data? I´ve increased it to 80ms.
		- 2018-05-21_veloc: 80ms did not work reliable so I´ve put "waitForMQTT" inside the for-loop and reduced it to 20ms again. now testing.
- [ ] / BSEN00002 / sometimes, the BME is not being recognized: "Could not find a valid BME280 sensor, check wiring!"
        - 2018-05-21_veloc: could really be the wiring... I am using cheap cables at the moment and after wiggeling them the sensor had been recognized, so maybe that could be an issue resolved by soldering the sensor correctly to the ESP?