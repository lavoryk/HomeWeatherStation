#include <Arduino.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include <time.h>

#include "WebServerImpl.h"
#include "Utilities.h"
#include "WifiConfig.h"
#include <WeatherSensor.h>

#define SEALEVELPRESSURE_HPA (1013.25)

WeatherSensor weatherSensor;
hd44780_I2Cexp lcd(0x27, 20, 4);

TaskHandle_t Task1;

const char* ssid = YOUR_WIFI_SSID;
const char* password = YOUR_WIFI_PASSWD;

void Task1code(void * parameter);

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 2 * 3600;
const int   daylightOffset_sec = 3600;

void setup() 
{
  lcd.init();

  Serial.begin(115200);
  Serial.println("HomeWeatherSation!!");
  
  weatherSensor.InitSensor();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Try to connect to WiFi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  SetUpWebServer(&weatherSensor);

  lcd.on();
  lcd.clear();

  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */

    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
}

void Task1code(void * parameter)
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  unsigned long nextWeatherDataUpdate = 0u;
  unsigned long nextTimeUpdate = 0u;
  bool print2Console = false;
  auto localIp = WiFi.localIP().toString();
  while (true)
  {
    print2Console = false;
    auto timePass = millis();
    if (nextWeatherDataUpdate < timePass)
    {
      nextWeatherDataUpdate += 10000u;
      print2Console = true;
      localIp = WiFi.localIP().toString();
      weatherSensor.ReadWeatherSersor();
    }
    
    if (nextTimeUpdate < timePass)
    {
      nextTimeUpdate += 1000u;
      PrintValues(weatherSensor, lcd, localIp, print2Console, timePass / 1000);
    }
  }
}

void loop() // core 1
{
  HandleWebServerClient();
}