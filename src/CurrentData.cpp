#include "CurrentData.h"

CurrentData::CurrentData()
{
    clear();
}

CurrentData::~CurrentData()
{
}

void CurrentData::clear()
{
    m_dateTime = -1;
    m_dayOfWeek = -1;
    
    m_temp = 0;
    m_tempFeelsLike = 0;

    m_windSpeed = -1;
    m_windDeg = -1;
    //m_windGust = -1;

    m_pressure = -1;
	m_humidity = -1;
	//m_dew_point = -1;
	m_uvi = -1;

    m_weatherId = -1;
    memset(&m_weatherIcon, 0, sizeof(m_weatherIcon));
}