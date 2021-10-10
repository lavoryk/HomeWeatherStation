#include "Utilities.h"
#include <Arduino.h>
#include <Wire.h>
#include <time.h>

#define SEALEVELPRESSURE_HPA (1013.25)

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

void PrintValues(
  const WeatherSensor& weatherSensor,
  const Currency& currency,
  hd44780& lcd,
  const String& localIp,
  bool updateWeatherData,
  bool updateCurrencyData,
  int uptimeSeconds)
{
  float temperature = weatherSensor.GetTemperature();
  float pressure = weatherSensor.GetPressure();

  /*Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");*/

  float humidity = weatherSensor.GetHumidity();

  if (updateWeatherData)
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

  char line[32] = {0};

  if (updateWeatherData)
  {
    sprintf(line, "T%+5.1f H%4.1f P%6.1f ", temperature, humidity, pressure);
    line[20] = 0;
    lcd.setCursor(0, 0);
    lcd.print(line);
  }
  
  lcd.setCursor(0, 1);
  time_t rawtime;
  time (&rawtime);
  struct tm* timeinfo = localtime (&rawtime);
  size_t written = strftime(line, 64, "%H:%M:%S", timeinfo);
  if(written == 0)
  {
      lcd.print("strftime error");
  }
  else
  {
    lcd.print(line);
  }
  
  sprintf(line, "  %02d:%02d:%02d", uptimeSeconds / 3600, (uptimeSeconds % 3600) / 60, uptimeSeconds % 60);
  lcd.print(line);
  if (updateCurrencyData)
  {
    lcd.setCursor(0, 2);
    sprintf(line, "N%5.2f P%5.2f/%5.2f", currency.GetExchangeRate24SaleRateNB(), currency.GetExchangeRate24PurchaseRate(), currency.GetExchangeRate24SaleRate());
    lcd.print(line);
  }
  if (updateWeatherData)
  {
    // WiFi
    lcd.setCursor(0, 3);
    lcd.print(localIp);
    PrintEmpty(lcd, 20 - localIp.length());
  }
}