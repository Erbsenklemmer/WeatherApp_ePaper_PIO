#pragma once

//original:#include <Arduino.h>

#include "CurrentData.h"
#include "HourlyData.h"
#include "DailyData.h"

#define hourlyForeCasts 3
#define dailyForeCasts 4

class OneCallData
{
public:
    OneCallData();
    ~OneCallData();

    void clear();

    signed char m_timezone_offset_in_half_hours;

    bool m_currentReadEnable;

    bool m_hourlyReadEnable;
    char m_hourlyCountRaw;
    byte m_hourlyCount;

    bool m_dailyReadEnable;
    char m_dailyCountRaw;
    byte m_dailyCount;

    CurrentData m_currentData;
    HourlyData m_hourlyData[hourlyForeCasts];
    DailyData m_dailyData[dailyForeCasts];
};