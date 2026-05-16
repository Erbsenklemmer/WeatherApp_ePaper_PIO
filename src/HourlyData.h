#pragma once

#include <Arduino.h>

class HourlyData
{
public:
    HourlyData();
    ~HourlyData();

    void clear();

    int m_dateTime;
    float m_temparature;
    float m_feels_like;
    char m_humidity;
    float m_uvi;
    float m_windSpeed;
    int m_windDegree;
    String m_description;
    char m_icon[5];
    int m_rain1h;
    int m_rain3h;
    int m_snow1h;
    int m_snow3h;

};