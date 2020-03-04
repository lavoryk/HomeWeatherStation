#include <Arduino.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#if defined ESP32
  #include <WiFi.h>
  #define configTime4Project configTzTime
  TaskHandle_t Task1;
  void Task1code(void * parameter);
#elif defined ESP8266
  #include <ESP8266WiFi.h>
  #define configTime4Project configTime
#endif

#ifdef USE_SD
  #include <SD.h>
  #include <FS.h>
#endif

#include "WebServerImpl.h"
#include "Utilities.h"
#include "WifiConfig.h"
#include <WeatherSensor.h>
#include <Currency.h>

WeatherSensor weatherSensor;
hd44780_I2Cexp lcd(0x27, 20, 4);

const char* ssid = YOUR_WIFI_SSID;
const char* password = YOUR_WIFI_PASSWD;
const char* ntpServer = "pool.ntp.org";

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
  
  // Kyiv time zone
  configTime4Project("UTC-02DST+01", ntpServer);

  SetUpWebServer(&weatherSensor);

  lcd.on();
  lcd.clear();

#if defined ESP32
  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */
#endif

#ifdef USE_SD
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
    #endif
    Currency currency;
    currency.ReadCurrency();
}

void HandleSensorsAndLCD()
{
  // Initialize one time
    static unsigned long nextWeatherDataUpdate = 0u;
    static unsigned long nextTimeUpdate = 0u;
    static bool updateWeatherData = false;
    static auto localIp = WiFi.localIP().toString();

    auto timePass = millis();
    if (nextWeatherDataUpdate < timePass)
    {
      nextWeatherDataUpdate += 10000u;
      updateWeatherData = true;
      localIp = WiFi.localIP().toString();
      weatherSensor.ReadWeatherSersor();
    }
    
    if (nextTimeUpdate < timePass)
    {
      nextTimeUpdate += 1000u;
      PrintValues(weatherSensor, lcd, localIp, updateWeatherData, timePass / 1000);
      updateWeatherData = false;
    }
}

#if defined ESP32
// core 2
void Task1code(void * parameter)
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  while (true)
  {
    HandleSensorsAndLCD();
  }
}
#endif

void loop() // core 1
{
  HandleWebServerClient();
#if defined ESP8266
  HandleSensorsAndLCD();
#endif
}