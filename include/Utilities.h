#pragma once
#include <Print.h>
#include <WeatherSensor.h>
#include <Currency.h>
#include <hd44780.h>

void PrintEmpty(Print&print, unsigned int count);
void PrintValues(const WeatherSensor& weatherSensor,
    const Currency& currency,
    hd44780& lcd,
    const String& localIp,
    bool updateWeatherData,
    bool updateCurrencyData,
    int uptimeSeconds);