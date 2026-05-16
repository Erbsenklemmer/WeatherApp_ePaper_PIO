#pragma once

#include <Arduino.h>

class DailyData
{
public:
	DailyData();
	~DailyData();

	void clear();

	int m_dateTime;
  int16_t m_dayOfWeek; 

	int m_sunRise;
	int m_sunSet;

	int m_moonRise;
	int m_moonSet;
	float m_moonPhase;

	int m_pressure;
	char m_humidity;
	float m_dewPoint;

	float m_windSpeed;
	float m_windDeg;
	float m_windGust;

	char m_cloudsInPercent;
	float m_probabilityOfPrecipitation;
	float m_rainMM;
	float m_snowMM;
	float m_uvi;

	//String m_summaryInEnglish;

	float m_tempDay;
	float m_tempMin;
	float m_tempMax;
	float m_tempNight;
	float m_tempEve;
	float m_tempMorn;

	int m_weatherId;
	String m_weatherMain;
	String m_weatherDescription;
	char m_weatherIcon[5];
};

