#pragma once
class WeatherSensor
{
public:
    WeatherSensor();
    void InitSensor();
    void ReadWeatherSersor();
    float GetTemperature() const
    {
        return m_temperature;
    }
    float GetPressure() const
    {
        return m_pressure;
    }
    float GetHumidity() const
    {
        return m_humidity;
    }
private:
    float m_temperature;
    float m_pressure;
    float m_humidity;
};