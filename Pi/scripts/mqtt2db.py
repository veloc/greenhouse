#!/usr/bin/env python
# September 2013
# by Matthew Bordignon, @bordignon on Twitter
#
# Simple Python script (v2.7x) that subscribes to a MQTT broker topic and inserts the topic into a mysql database
# This is designed for the http://mqttitude.org/ project backend
#

import MySQLdb
import paho.mqtt.client as mqtt 
import json
import time

#mosquitto broker config
broker = 'localhost'
broker_port = 1883
topic = 'test'
broker_topic = '/'+topic+'/#'
broker_clientid = 'mqttuide2mysqlScript'
#mysql config
mysql_server = 'localhost'
mysql_username = 'broker'
mysql_passwd = 'broker'
mysql_db = 'TEST'
mysql_tablename = 'tbl_test'
mysql_sensor_col = 'sensor'
mysql_date_col = 'date'
mysql_value_col = 'wert'

dbg = 'DEBUG: '

# Open database connection
db = MySQLdb.connect(mysql_server, mysql_username, mysql_passwd, mysql_db)
# prepare a cursor object using cursor() method
cursor = db.cursor()

# to make sure, sth is utf-8 coded, unicode failed to be stored into the db because of (u'string')
def ensure_str(s):
    if isinstance(s, unicode):
	  s = s.encode('utf-8')
    return s

#def on_connect(mosq, obj, rc):
# modified line containing "self"
def on_connect(self, mosq, obj, rc):
    print("rc: "+str(rc))

def on_message(mosq, obj, msg):
    print ("")
    print(dbg+"Topic is: "+msg.topic+" QOS is: "+str(msg.qos)+" Message is: "+str(msg.payload))
	# defining lists and variables
    vars_to_sql = []
    keys_to_sql = []
#    list = []
    value = 0.0
	
    str_msg = str(msg.payload)
    key = ensure_str(msg.topic)
	
	# making sure, every subtopic of topic is being reacted to
    if key.startswith("test"):
      #value = time.strftime('%Y-%m-%d %H:%M:%S')
      #value = time.time()
      #print (dbg+"setting date (value) to: "+str(value))
      
      #value_type = type(value)
	
	  # filling "vars_to_sql" with the msq.topic encoded to utf-8
      vars_to_sql.append(ensure_str(msg.topic))
      #vars_to_sql.append(value)
      vars_to_sql.append(msg.payload.encode('ascii', 'ignore'))

      keys_to_sql = (mysql_sensor_col+', '+mysql_value_col)

      try:
       # Execute the SQL command 
       queryText = "INSERT INTO "+mysql_tablename+"(%s) VALUES %r"
       queryArgs = (keys_to_sql, tuple(vars_to_sql))
       print (dbg+"Query is: "+(queryText % queryArgs))
       cursor.execute(queryText % queryArgs)
       print('Successfully Added record to mysql')
       db.commit()
      except MySQLdb.Error, e:
        try:
            print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
        except IndexError:
            print "MySQL Error: %s" % str(e)
        # Rollback in case there is any error
        db.rollback()
        print('ERROR adding record to MYSQL')

def on_publish(mosq, obj, mid):
    print("mid: "+str(mid))

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed: "+str(mid)+" "+str(granted_qos))

def on_log(mosq, obj, level, string):
    print(string)

# If you want to use a specific client id, use
#mqttc = mosquitto.Mosquitto(broker_clientid)
# but note that the client id must be unique on the broker. Leaving the client
# id parameter empty will generate a random id for you.
#mqttc = mosquitto.Mosquitto()
mqttc = mqtt.Client()
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe
# Uncomment to enable debug messages
mqttc.on_log = on_log

mqttc.connect(broker, broker_port, 60)
mqttc.subscribe(broker_topic, 0)

rc = 0
while rc == 0:
    rc = mqttc.loop()

print("rc: "+str(rc))

# disconnect from server
print ('Disconnected, done.')
db.close()
