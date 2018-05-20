/****************************************************/

#include <PubSubClient.h>     // MQTT libs
#include "Adafruit_BME280.h"  // https://github.com/Takatsuki0204/BME280-I2C-ESP32 BME280 Sensor libs
#include <WiFi.h>             // wifi-lib (important to use the correct one for esp32!)
#include "esp_deep_sleep.h"   // deepsleep lib

/************************* WiFi Access Point *********************************/

const char* WLAN_SSID = "receiver";
const char* WLAN_PASS = "kensentme!";

WiFiClient espClient;

/************************* MQTT Setup *********************************/

#define MQTT_SERVER     "4.3.2.1"
#define MQTT_SERVERPORT 1883
#define MQTT_USERNAME   "esp1"
#define MQTT_PASSWORD   "esp1"
#define MQTT_TOPIC      "test"
#define MQTT_AIR_PRES_TOPIC "test/air/pres"
#define MQTT_AIR_TEMP_TOPIC "test/air/temp"
#define MQTT_AIR_HUM_TOPIC  "test/air/hum"
#define MQTT_SOIL_HUM_TOPIC  "test/soil/hum"

int waitForMQTT = 80; // Delay after transmitting messages. 20ms seem to be enough for one transmission

/******** DFROBOT Capacitive Soil Moisture Sensor V1.0 Setup ************/

#define CSMS_PIN 34

const int CSMS_Max = 2915; // Capacitive Soil Moisture Sensor in the air 
const int CSMS_Min = 1412; // Capacitive Soil Moisture Sensor in water

int CSMS_100Percent = (CSMS_Max - CSMS_Min);
int CSMS_SoilValue = 0;

/************************* BME280 Setup *********************************/

#define ALTITUDE 147.0 // Altitude in Göttingen, Germany, needed to calculate getPressure()
#define I2C_SDA 26
#define I2C_SCL 27
#define BME280_ADDRESS 0x76  //If the sensor does not work, try both addresses: 0x76 and 0x77 

Adafruit_BME280 bme(I2C_SDA, I2C_SCL);

/*************************** Variables and other Stuff ******************/

byte mac[6];

String bat_str;
char MQTT_MESSAGE[50];
String air_pres_str;
String air_hum_str;
String air_temp_str;
String soil_hum_str;
int num_of_sensors = 4; // how many sensors do we have?

unsigned int SleepDuration = 0;

const int UpdateIntervalHours = 1;    // How long should the ESP sleep - WARNING: using an "unsigned int" for "SleepDuration" and "UpdateInterval" allows for roughly 70 minutes of sleeptime...
const int UpdateIntervalMinutes = 0;
const int UpdateIntervalSeconds = 0; // How long should the ESP sleep in seconds?

const int SleepSecond = 1000000;
//const int SleepSecond = (0.33 * 3 * 1000000); // 0.33 * 60 * 1000000; = 20 sec, so 0.33 * 3 * 1000000 should be 1 sec

const int ErrorSleep = (SleepSecond*30);
const int SleepBetweenUpdates = ((UpdateIntervalHours*60*60) + (UpdateIntervalMinutes*60) + (UpdateIntervalSeconds));

const unsigned int UpdateInterval = (SleepSecond*SleepBetweenUpdates);  


float temperature = 0;
float humidity = 0;
float pressure = 0;

/*************************** Sketch Code ************************************/

/************************* Functions for CSMS *******************************/

int getSoilHum() {
  int value = (100 - ((( analogRead (CSMS_PIN) ) - CSMS_Min) / (CSMS_100Percent / 100)));
  /*
   * Hopefully this calculation is correct:
   * I take the maximum value ans substract the minimum value. that leaves me with the range:
   * CSMS_Max - CSMS_Min = CSMS_100Percent
   * 760      - 400      = 360
   * so 360 is 100% of the possible range.
   * 
   * Then I read the actual analog value of the sensor and substract the minimum so I have an absolute value (lets say 240):
   * analogRead CSMS_PIN - CSMS_Min = someValue
   *            640      - 400      = 240
   *            
   * Next, I divide someValue by 1% to get the "missing" humidity since the value increases the drier the soil gets:
   * someValue / (CSMS_100Percent / 100) = neg.PercentHum
   * 240       / 3,6 =                   = 66
   * 
   * So now I assume that the soil is 66 % "dry". To calculate the % humidity, I simply substract the "dry" value from an 100%:
   * 100% - neg.PercentHum = actual_humidity
   * 100  - 66             = 34
   * 
   * Caclulation may not be 100% accurate because I actually store floats as ints...
   */
   return value;
}

/************************* Functions for MQTT *******************************/

 void callback(char* topic, byte* payload, unsigned int length) {
 // handle message arrived
}

PubSubClient client(MQTT_SERVER, MQTT_SERVERPORT, callback, espClient);

 void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
 }

 /************************ Setup ********************************************/

 void setup() {
  Serial.begin(115200);
  Serial.println("MQTT test");

  initSensor();
  // Connect to WiFi access point.
  setup_wifi();

  client.setServer(MQTT_SERVER, MQTT_SERVERPORT);
 }

/**************************** Main Loop ***********************************/

 void loop() {

  //DEBUG
  Serial.println("");
  Serial.println("DEBUG START");
  Serial.println("Reading directly from Sensor: " + String(analogRead (CSMS_PIN)));
  Serial.println("Substracting CSMS_Min: " + String((analogRead (CSMS_PIN)) - CSMS_Min));
  Serial.println("DEBUG END");
  Serial.println("");
  Serial.println("AirHum: "+String(getHumidity()));
  Serial.println("AirTemp: "+String(getTemperature()));
  Serial.println("AirPres: "+String(getPressure()));
  Serial.println("SoilHum: "+String(getSoilHum()));
  Serial.println("");
  //DEBUG
  
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  for (int i = 0; i <= num_of_sensors; i++) {
    if (i == 1) {
      air_hum_str = String(getHumidity()); 
      air_hum_str.toCharArray(MQTT_MESSAGE, air_hum_str.length() +1);
      client.publish(MQTT_AIR_HUM_TOPIC, MQTT_MESSAGE, true);
      Serial.println(String(MQTT_AIR_HUM_TOPIC)+" "+String(MQTT_MESSAGE));
    }

    if (i == 2) {
      air_temp_str = String(getTemperature()); 
      air_temp_str.toCharArray(MQTT_MESSAGE, air_temp_str.length() +1);
      client.publish(MQTT_AIR_TEMP_TOPIC, MQTT_MESSAGE, true);
      Serial.println(String(MQTT_AIR_TEMP_TOPIC)+" "+String(MQTT_MESSAGE));
    }

    if (i == 3) {
      air_pres_str = String(getPressure());  //converting the float from getPressure() to a string
      air_pres_str.toCharArray(MQTT_MESSAGE, air_pres_str.length() + 1); //packaging up the data to publish to mqtt
      client.publish(MQTT_AIR_PRES_TOPIC, MQTT_MESSAGE, true);
      Serial.println(String(MQTT_AIR_PRES_TOPIC)+" "+String(MQTT_MESSAGE));
    }

    if (i == 4) {
      soil_hum_str = String(getSoilHum());  //converting the float from getSoilHum() to a string
      soil_hum_str.toCharArray(MQTT_MESSAGE, soil_hum_str.length() + 1); //packaging up the data to publish to mqtt
      client.publish(MQTT_SOIL_HUM_TOPIC, MQTT_MESSAGE, true);
      Serial.println(String(MQTT_SOIL_HUM_TOPIC)+" "+String(MQTT_MESSAGE));
    }

  }

  Serial.println("Sleeping after transmitting...");
  delay(waitForMQTT); //needed because mqtt messages where not being received without delay.
  
  //going to sleep....
  Serial.println("Setting SleepDuration = UpdateInterval");
  SleepDuration = UpdateInterval;
  do_sleep();
 }


/******************* Function for connecting to WLAN ************************/

 void setup_wifi() {
  delay(10);
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  int loopcount = 0;
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    loopcount++;
    if (loopcount==5) {
      Serial.println("going to sleep because no wifi AP could be reached...");
      SleepDuration = ErrorSleep;
      do_sleep();
    }
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  
 }

/*************************** Functions for BME280 *************************/

void initSensor()
{
  bool status = bme.begin(BME280_ADDRESS);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    //while (1);      // this will halt the system if no sensor could be detected - which I don´t want.
    SleepDuration = ErrorSleep;
    do_sleep();       // going to sleep and see if the sensor responds next time...
  }
}

float getTemperature()
{
  temperature = bme.readTemperature();
}

float getHumidity()
{
  humidity = bme.readHumidity();
}

float getPressure()
{
  pressure = bme.readPressure();
  pressure = bme.seaLevelForAltitude(ALTITUDE,pressure);
  pressure = pressure/100.0F;
}

/******************************* Sleep Function ******************************/

void do_sleep()
{
  esp_deep_sleep_enable_timer_wakeup(SleepDuration);
  
  Serial.println("Going to sleep for "+String(SleepDuration/SleepSecond)+" seconds now...");
  //DEBUG
  Serial.println("This is calculated by SleepDuration (" +String(SleepDuration)+ ") divided by SleepSecond (" +String(SleepSecond)+ ")");
  Serial.println("DEBUG: SleepDuration is set dynamically but should right now be set to UpdateInterval (" +String(UpdateInterval)+ ")");
  Serial.println("DEBUG: UpdateInterval is calculated by SleepSecond (" +String(SleepSecond)+ ") times SleepBetweenUpdates (" +String(SleepBetweenUpdates)+ ")");
  Serial.println("DEBUG: SleepBetweenUpdates in turn is calculated by (UpdateIntervalHours (" +String(UpdateIntervalHours)+ ") times 60 times 60) + (UpdateIntervalMinutes (" +String(UpdateIntervalMinutes)+ ") times 60) + (UpdateIntervalSeconds (" +String(UpdateIntervalSeconds)+ "))");
  //DEBUG
    
  esp_deep_sleep_start();
}
