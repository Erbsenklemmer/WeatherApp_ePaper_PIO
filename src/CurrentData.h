#pragma once

#include <Arduino.h>

class CurrentData
{
public:
    CurrentData();
    ~CurrentData();

    void clear();

    int m_dateTime;
    int m_dayOfWeek;

    float m_temp;
    float m_tempFeelsLike;

	int m_pressure;
	int m_humidity;
	//float m_dew_point;
	float m_uvi;

    float m_windSpeed;
    float m_windDeg;
    //float m_windGust;

    int m_weatherId;
	String m_weatherMain;
	String m_weatherDescription;
	char m_weatherIcon[5];
};

