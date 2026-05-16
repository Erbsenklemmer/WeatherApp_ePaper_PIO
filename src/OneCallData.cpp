
#include "OneCallData.h"

OneCallData::OneCallData()
{
    clear();
}

OneCallData::~OneCallData()
{}

void OneCallData::clear()
{
    m_timezone_offset_in_half_hours = 0;

    m_currentReadEnable = false;

    m_hourlyCountRaw = -1;
    m_hourlyCount = -1;
    m_hourlyReadEnable = false;

    m_dailyReadEnable = false;
    m_dailyCountRaw = -1;
    m_dailyCount = -1;

    m_currentData.clear();

    for (int i = 0; i < hourlyForeCasts; i++)
    {
        m_hourlyData[i].clear();
    }

    for (int i = 0; i < dailyForeCasts; i++)
    {
        m_dailyData[i].clear();
    }
}

