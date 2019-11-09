/****************************************************/

#include <PubSubClient.h>     // MQTT libs
#include "Adafruit_BME280.h"  // https://github.com/Takatsuki0204/BME280-I2C-ESP32 BME280 Sensor libs
#include <WiFi.h>             // wifi-lib (important to use the correct one for esp32!)
#include "esp_deep_sleep.h"   // deepsleep lib

/********************** PIN Layout ********************/

//                                            +-\/-+
//                              VIN3.3V  3.3V |    | GND GND
//                   (RESET)    EN       EN   |    | P23 GPIO23  (VPSIMOSI)
//                   (ADC0)     GPIO36   SVP  |    | P22 GPIO22
//                   (ADC3)     GPIO39   SVN  |    | TX  GPIO1   (TX0)
// *CSMS*            (ADC6)     GPIO34   P34  |    | RX  GPIO3   (RX0)
//                   (ADC7)     GPIO35   P35  |    | P21 GPIO21
//          [TOUCH9] (ADC4)     GPIO32   P32  |    | GND GND
//          [TOUCH8] (ADC5)     GPIO33   P33  |    | P19 GPIO19 (VSPIMISO)
//                   (ADC18)    GPIO25   P25  |    | P18 GPIO18 (VSPI SCK)
// *BME280*          (ADC19)    GPIO26   P26  |    | P5  GPIO5  (VSPI SS)
// *BME280* [TOUCH7] (ADC17)    GPIO27   P27  |    | P17 GPIO17
//          [TOUCH6] (ADC16)    GPIO14   P14  |    | P16 GPIO16
//          [TOUCH5] (ADC15)    GPIO12   P12  |    | P4  GPIO4  (ADC10)    [TOUCH0]
//                              GND      GND  |    | P0  GPIO0  (ADC11)    [TOUCH1]
//          [TOUCH4] (ADC14)    GPIO13   P13  |    | P2  GPIO2  (ADC12)    [TOUCH2]
//                   (FLASH D2) GPIO9    SD2  |    | P15 GPIO15 (ADC13)    [TOUCH3]
//                   (FLASH D3) GPIO10   SD3  |    | SD1 GPIO8  (FLASH D1)
//                   (FLASHCMD) GPIO11   CMD  |    | SD0 GPIO7  (FLASH D0)
//                              VIN 5V   5V   |    | CLK GPIO6  (FLASHSCK)
//                                            +----+

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

int waitForMQTT = 20; // Delay after transmitting messages. 20ms seemed to be enough for one transmission initially

/******** DFROBOT Capacitive Soil Moisture Sensor V1.0 Setup ************/

#define CSMS_PIN 34

const int CSMS_Max = 2915; // Capacitive Soil Moisture Sensor in the air 
const int CSMS_Min = 1412; // Capacitive Soil Moisture Sensor in water

int CSMS_100Percent = (CSMS_Max - CSMS_Min);
int CSMS_SoilValue = 0;

/************************* BME280 Setup *********************************/

#define ALTITUDE 147.0 // Altitude in target location, needed to calculate getPressure()
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

// How long should the ESP sleep - WARNING: using an "unsigned int" for "SleepDuration" and "UpdateInterval" allows for roughly 70 minutes of sleeptime...
unsigned int SleepDuration = 0;       // with an unsigned int we actually can go to 4,294,967,295 ms

const int UpdateIntervalHours = 1;    // How many hours? maximum of 1...
const int UpdateIntervalMinutes = 0;  // How many minutes?
const int UpdateIntervalSeconds = 0;  // How many seconds?

const int SleepSecond = 1000000;

const int ErrorSleep = (SleepSecond*30);
const int SleepBetweenUpdates = ((UpdateIntervalHours*60*60) + (UpdateIntervalMinutes*60) + (UpdateIntervalSeconds));

const unsigned int UpdateInterval = (SleepSecond*SleepBetweenUpdates);  

float temperature = 0;
float humidity = 0;
float pressure = 0;

/********************************** MQTT Setup **************************/

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
      Serial.println("");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
 }

/*************************** Sketch Code ************************************/
/************************ Setup ********************************************/

 void setup() {
  Serial.begin(115200);
  initBME();                                        // init BME-Sensor
  setup_wifi();                                     // setup WiFi
  client.setServer(MQTT_SERVER, MQTT_SERVERPORT);   // setup MQTT
 }

/**************************** Main Loop ***********************************/

 void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  for (int i = 0; i <= num_of_sensors; i++) {       // go to all the connected sensors... there has to be a prettier way to do this...
    if (i == 1) {
      air_hum_str = String(getHumidity()); 
      air_hum_str.toCharArray(MQTT_MESSAGE, air_hum_str.length() +1);
      client.publish(MQTT_AIR_HUM_TOPIC, MQTT_MESSAGE, true);
      Serial.println(String(MQTT_AIR_HUM_TOPIC)+" "+String(MQTT_MESSAGE));
      delay(waitForMQTT); //needed because mqtt messages where not being received without delay.
    }

    if (i == 2) {
      air_temp_str = String(getTemperature()); 
      air_temp_str.toCharArray(MQTT_MESSAGE, air_temp_str.length() +1);
      client.publish(MQTT_AIR_TEMP_TOPIC, MQTT_MESSAGE, true);
      Serial.println(String(MQTT_AIR_TEMP_TOPIC)+" "+String(MQTT_MESSAGE));
      delay(waitForMQTT); //needed because mqtt messages where not being received without delay.
    }

    if (i == 3) {
      air_pres_str = String(getPressure());  //converting the float from getPressure() to a string
      air_pres_str.toCharArray(MQTT_MESSAGE, air_pres_str.length() + 1); //packaging up the data to publish to mqtt
      client.publish(MQTT_AIR_PRES_TOPIC, MQTT_MESSAGE, true);
      Serial.println(String(MQTT_AIR_PRES_TOPIC)+" "+String(MQTT_MESSAGE));
      delay(waitForMQTT); //needed because mqtt messages where not being received without delay.
    }

    if (i == 4) {
      soil_hum_str = String(getSoilHum());  //converting the float from getSoilHum() to a string
      soil_hum_str.toCharArray(MQTT_MESSAGE, soil_hum_str.length() + 1); //packaging up the data to publish to mqtt
      client.publish(MQTT_SOIL_HUM_TOPIC, MQTT_MESSAGE, true);
      Serial.println(String(MQTT_SOIL_HUM_TOPIC)+" "+String(MQTT_MESSAGE));
      delay(waitForMQTT); //needed because mqtt messages where not being received without delay.
    }

  }

  Serial.println("Sleeping after transmitting...");
  
  //going to sleep....
  Serial.println("Setting SleepDuration = UpdateInterval");
  SleepDuration = UpdateInterval;
  do_sleep();
 }


/******************* Function for WLAN-related stuff ************************/

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

  printMAC();
 }

void printMAC()
{
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

void initBME()
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

  /*
  //DEBUG
  Serial.println("");
  Serial.println("DEBUG: This is calculated by SleepDuration (" +String(SleepDuration)+ ") divided by SleepSecond (" +String(SleepSecond)+ ")");
  Serial.println("DEBUG: SleepDuration is set dynamically but should right now be set to UpdateInterval (" +String(UpdateInterval)+ ")");
  Serial.println("DEBUG: UpdateInterval is calculated by SleepSecond (" +String(SleepSecond)+ ") times SleepBetweenUpdates (" +String(SleepBetweenUpdates)+ ")");
  Serial.println("DEBUG: SleepBetweenUpdates in turn is calculated by (UpdateIntervalHours (" +String(UpdateIntervalHours)+ ") times 60 times 60) + (UpdateIntervalMinutes (" +String(UpdateIntervalMinutes)+ ") times 60) + (UpdateIntervalSeconds (" +String(UpdateIntervalSeconds)+ "))");
  Serial.println("");
  //DEBUG
  */
    
  esp_deep_sleep_start();
}

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
