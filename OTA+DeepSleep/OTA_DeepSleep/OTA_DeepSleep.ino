/************************* including libraries *********************************/

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "esp_deep_sleep.h"   // deepsleep lib

/************************* WiFi Access Point *********************************/

const char* ssid = "receiver";
const char* password = "kensentme!";

/*************************** Variables and other Stuff ******************/

/*************************** Sleeping related variables *****************/

int SleepDuration = 0;

const int UpdateIntervalHours = 0;    // How long should the ESP sleep
const int UpdateIntervalMinutes = 0;
const int UpdateIntervalSeconds = 30; // How long should the ESP sleep in seconds?

const int SleepSecond = (0.33 * 3 * 1000000); // 0.33 * 60 * 1000000; = 20 sec, so 0.33 * 3 * 1000000 should be 1 sec

const int ErrorSleep = (SleepSecond*30);
const int SleepBetweenUpdates = ((UpdateIntervalHours*60*60) + (UpdateIntervalMinutes*60) + (UpdateIntervalSeconds));

const int UpdateInterval = (SleepSecond*SleepBetweenUpdates);  

/*************************** Sketch Code ************************************/

/*************************** Setup ******************************************/
 
void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

/**************************** Main Loop ***********************************/

void loop() {

  // waiting 15s since boottime for an OTA update
  while (millis() < 15000){ 
    ArduinoOTA.handle();
    Serial.println("Handling at " + String(millis()) + " millis...");
  }

  //going to sleep....
  SleepDuration = UpdateInterval;
  do_sleep();
  
}

/******************************* Sleep Function ******************************/

void do_sleep()
{
  esp_deep_sleep_enable_timer_wakeup(SleepDuration);
  Serial.println("Going to sleep for "+String(SleepDuration/990000)+" seconds now...");
  esp_deep_sleep_start();
}
