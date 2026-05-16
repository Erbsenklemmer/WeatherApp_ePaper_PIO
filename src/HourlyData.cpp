
#include "HourlyData.h"

HourlyData::HourlyData()
{
    clear();
}

HourlyData::~HourlyData()
{}

void HourlyData::clear()
{
    m_dateTime = -1;
    m_temparature = 0;
    m_feels_like = 0;
    m_humidity = -1;
    m_uvi = -1;
    m_windSpeed = -1;
    m_windDegree = -1;
    memset(&m_icon, 0, sizeof(m_icon));
    m_rain1h = -1;
    m_rain3h = -1;
    m_snow1h = -1;
    m_snow3h = -1;
 }