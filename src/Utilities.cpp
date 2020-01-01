#include "Utilities.h"
#include <Arduino.h>
#include <Wire.h>
#include <time.h>

void PrintEmpty(Print&print, unsigned int count)
{
    if (count > 0)
    {
        static char filler[21] = {0};
        memset(filler, ' ', 20);
        count = min(20u, count);
        filler[count] = 0;
        print.print(filler);
    } 
}

void PrintValues(const WeatherSensor& weatherSensor, hd44780& lcd, const String& localIp, bool print2Console, int uptimeSeconds)
{
  float temperature = weatherSensor.GetTemperature();
  float pressure = weatherSensor.GetPressure();

  /*Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");*/

  float humidity = weatherSensor.GetHumidity();

  if (print2Console)
  {
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.println();
  }

  // lcd.setCursor(0, 0);
  // lcd.print("Temp*C|Hum % |P hPa ");

  char line[64] = {0};
  sprintf(line, "T%+5.1f H%4.1f P%6.1f ", temperature, humidity, pressure);
  line[21] = 0;
  lcd.setCursor(0, 0);
  lcd.print(line);
  
  lcd.setCursor(0, 1);
  // TODO: Optimize
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    
  }
  else
  {
    lcd.print(&timeinfo, "%H:%M:%S");
  }
  sprintf(line, "  %02d:%02d:%02d", uptimeSeconds / 3600, (uptimeSeconds % 3600) / 60, uptimeSeconds % 60);
  lcd.print(line);
  
  lcd.setCursor(0, 2);
  lcd.print("-Hello world-");

  // WiFi
  lcd.setCursor(0, 3);
  lcd.print(localIp);
  PrintEmpty(lcd, 20 - localIp.length());
}