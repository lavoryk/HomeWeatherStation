#pragma once
#include <Print.h>
#include <WeatherSensor.h>
#include <hd44780.h>

void PrintEmpty(Print&print, unsigned int count);
void PrintValues(const WeatherSensor& weatherSensor, hd44780& lcd, const String& localIp, bool print2Console, int uptimeSeconds);