#include "DailyData.h"

DailyData::DailyData()
{
    clear();
}

DailyData::~DailyData()
{
}

void DailyData::clear()
{
    m_dateTime = -1;
    m_dayOfWeek = -1;

    m_sunRise = -1;
    m_sunSet = -1;

    m_moonRise = -1;
    m_moonSet = -1;
    m_moonPhase = -1;

    m_pressure = -1;
    m_humidity = 255;
    m_dewPoint = -1;

    m_windSpeed = -1;
    m_windDeg = -1;
    m_windGust = -1;

    m_cloudsInPercent = 0;
    m_probabilityOfPrecipitation = -1;
    m_rainMM = -1;
    m_snowMM = -1; 
    m_uvi = -1;

    m_tempDay = -1;
    m_tempMin = -1;
    m_tempMax = -1;
    m_tempNight = -1;
    m_tempEve = -1;
    m_tempMorn = -1;

    m_weatherId = -1;
    memset(&m_weatherIcon, 0, sizeof(m_weatherIcon));
}