#include <WeatherSensor.h>
#include <Adafruit_BME280.h>
namespace
{
    Adafruit_BME280 bme; // I2C
}

WeatherSensor::WeatherSensor() :
    m_temperature(0.0f),
    m_pressure(0.0f),
    m_humidity(0.0f)
{

}

void WeatherSensor::InitSensor()
{
    bool status = bme.begin(0x76);
    if (!status) 
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
}

void WeatherSensor::ReadWeatherSersor()
{
    m_temperature = bme.readTemperature();
    m_pressure = bme.readPressure() / 100.0f;
    m_humidity = bme.readHumidity();
}
