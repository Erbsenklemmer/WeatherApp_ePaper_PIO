#include "HardwareSerial.h"
#include "OneCallListener.h"

//#include "../ArduinoC++Fake/Serial.h"
//original:#include <UnixTime.h>

OneCallListener::OneCallListener()
    : addIndent("    ")
    , dump(true) {
    Reset();
}

OneCallListener::~OneCallListener() {
}

void OneCallListener::Reset() {
    m_indent = 0;
    m_objectDepth = -1;
    m_arrayDepth = -1;
    m_lastKeyName.clear();
    m_lastObjectName.clear();

    m_data.clear();
}

void OneCallListener::whitespace(char c) {
    if (dump)
    {
        String s = strIndent + "whitespace: \'" + c + "\'";
#ifdef WIN32
        //Serial.println(s.c_str());
#else
        Serial.println();
#endif
    }
}

void OneCallListener::startDocument() {

    m_lastKeyName = "document";
    //if (dump)
    //  Serial.println(strIndent + "start document");

    strIndent += addIndent;
}

void OneCallListener::key(String key) {
    //if (dump)
    //  Serial.println(strIndent + "key " + m_lastKeyName + ": " + key);

    m_lastKeyName = key;
}

void OneCallListener::value(String value) {

    //if (dump)
    //  Serial.println(strIndent + "value: " + value);

    //     UnixTime unixTime(1);
    //     unixTime.day = unixTime.month = unixTime.year = unixTime.hour = unixTime.minute = unixTime.second = -1;
    //     unixTime.getDateTime(unixDateTime);

    //     if (dump) {
    //         Serial.println(String(unixTime.day) + '.' + String(unixTime.month) + '.' + String(unixTime.year) + ' '
    //       + String(unixTime.hour) + ':' + String(unixTime.minute) + ':' + String(unixTime.second));
    //     }

    if (m_lastKeyName == "timezone_offset")
    {
        m_data.m_timezone_offset_in_half_hours = (signed char)(value.toInt() / 1800);
    }

    if (m_data.m_currentReadEnable == true)
    {
        if (m_objectDepth == 2 && m_stackObjectName[m_objectDepth] == "weather")
        {
            ReadCurrentWeather(value);
        }
        else
        {
            ReadCurrent(value);
        }
    }
    else if (m_data.m_hourlyReadEnable == true && (m_data.m_hourlyCount < hourlyForeCasts))
    {
        if (m_arrayDepth == 0 && m_stackObjectName[m_objectDepth] == "hourly")
        {
            if (m_arrayDepth < m_stackSize)
            {
                ReadHourly(value);
            }
        }
        else if (m_arrayDepth == 1 && m_objectDepth > 1 && m_stackObjectName[m_objectDepth - 1] == "hourly" && m_stackObjectName[m_objectDepth] == "weather")
        {
            if (m_arrayDepth < m_stackSize)
            {
                ReadHourlyWeater(value);
            }
        }
        else if (m_arrayDepth == 0 && m_stackObjectName[m_objectDepth - 1] == "hourly" && m_stackObjectName[m_objectDepth] == "rain")
        {
            if (m_arrayDepth < m_stackSize)
            {
                ReadHourlyRain(value);
            }
        }
        else if (m_arrayDepth == 0 && m_stackObjectName[m_objectDepth - 1] == "hourly" && m_stackObjectName[m_objectDepth] == "snow")
        {
            if (m_arrayDepth < m_stackSize)
            {
                ReadHourlySnow(value);
            }
        }
    }
    else if (m_data.m_dailyReadEnable == true && (m_data.m_dailyCount < dailyForeCasts))
    {
        if (m_arrayDepth < m_stackSize)
        {
            if (m_objectDepth == 2 && m_stackObjectName[m_objectDepth] == "temp")
            {
                ReadDailyTemp(value);
            }
            else if (m_objectDepth == 2 && m_stackObjectName[m_objectDepth] == "feels_like")
            {
                //ReadDailyFeelsLike(value);?
            }
            else if (m_objectDepth == 2 && m_stackObjectName[m_objectDepth] == "weather")
            {
                ReadDailyWeather(value);
            }
            else
            {
                ReadDaily(value);
            }
        }
    }

    m_lastKeyName.clear();
}

void OneCallListener::startArray() {
    //if (dump)
    //  Serial.println(strIndent + "start array " + m_lastObjectName);

    strIndent += addIndent;
    m_arrayDepth++;
}

void OneCallListener::endArray() {
    strIndent = strIndent.substring(addIndent.length());

    m_arrayDepth--;

    //if (dump)
    //  Serial.println(strIndent + "end array " + m_lastObjectName);
}

void OneCallListener::startObject() {

    ++m_objectDepth;

    strIndent += addIndent;

    PushObjectName(m_lastKeyName);
    DumpStackObjectName();

    if (m_lastObjectName == "current")
    {
        m_data.m_currentReadEnable = true;
    }
    else if (m_lastObjectName == "hourly") 
    {
        m_data.m_hourlyCountRaw++;

        if (m_data.m_hourlyCountRaw > 0 && m_data.m_hourlyCountRaw % 6 == 0)
        {
            m_data.m_hourlyCount++;
            m_data.m_hourlyReadEnable = true;
        }
        else
        {
           m_data.m_hourlyReadEnable = false;
        }
    }
    else if (m_lastObjectName == "daily")
    {
        m_data.m_dailyCountRaw++;

        if (m_data.m_dailyCountRaw < dailyForeCasts)
        {
            m_data.m_dailyCount++;
            m_data.m_dailyReadEnable = true;
        }
        else
        {
            m_data.m_dailyReadEnable = false;
        }
    }

    m_lastKeyName.clear();

    //if (dump)
    //  Serial.println(strIndent + "start object \"" + m_lastObjectName + "\" depth: " + String(m_objectDepth));
}

void OneCallListener::endObject() {
    strIndent = strIndent.substring(addIndent.length());

    if (m_lastObjectName == "document")
    {
        return;
    }
    else if (m_lastObjectName == "current")
    {
        m_data.m_currentReadEnable = false;
    }
    else if (m_lastObjectName == "hourly")
    {
        m_data.m_hourlyReadEnable = false;
    }
    else if (m_lastObjectName == "daily")
    {
        m_data.m_dailyReadEnable = false;
    }

    --m_objectDepth;

    m_lastKeyName.clear();
    m_lastObjectName = PopObjectName();

    DumpStackObjectName();

    //if (dump)
    //  Serial.println(strIndent + "end object \"" + m_lastObjectName + "\" depth: " + String(m_objectDepth));
}

void OneCallListener::endDocument() {
    strIndent = strIndent.substring(addIndent.length());
    //if (dump)
    //  Serial.println(strIndent + "end document");
}

void OneCallListener::PushObjectName(const String& name) {
    //if (dump)
    //    Serial.println(strIndent + "pushed: '" + name + "' at depth: " + String(m_objectDepth));

    if (name.isEmpty()) {
        if (m_objectDepth < m_stackSize - 1)
            m_lastObjectName = m_stackObjectName[m_objectDepth];
        return;
    }
    if (m_objectDepth < m_stackSize - 1 && m_objectDepth >= 0) {
        m_stackObjectName[m_objectDepth] = name;
        m_lastObjectName = name;
    }
    else {
        //  Serial.println("error in push");
    }
}

const String OneCallListener::PopObjectName() {
    if (m_objectDepth >= 0) {
        String ret = m_stackObjectName[m_objectDepth];
        m_lastObjectName = ret;

        return ret;
    }
    else {
        //Serial.println("error in pop");
        return "error";
    }
    return "";
}

void OneCallListener::DumpStackObjectName() {
    if (dump == false)
        return;

    //for (int i = 0; i < m_stackSize; ++i) {
    //  Serial.print("'" + m_stackObjectName[i] + "' ");
    //}
    //Serial.println("");
}

const int16_t OneCallListener::CalcDayOfWeek(int unixTimeRaw)
{
  int unixTime = unixTimeRaw + (m_data.m_timezone_offset_in_half_hours * 1800);
  int16_t dayOfWeek = (int(unixTime / 86400.0) + 4) % 7;
  return dayOfWeek;
}

void OneCallListener::ReadCurrent(const String& value)
{
    if (m_lastKeyName == "dt")
    {
        m_data.m_currentData.m_dateTime = value.toInt();
        m_data.m_currentData.m_dayOfWeek = CalcDayOfWeek(value.toInt());
    }
    else if (m_lastKeyName == "temp")
    {
        m_data.m_currentData.m_temp = value.toFloat();
    }
    else if (m_lastKeyName == "feels_like")
    {
        m_data.m_currentData.m_tempFeelsLike = value.toFloat();
    }
    else if (m_lastKeyName == "wind_speed")
    {
        m_data.m_currentData.m_windSpeed = value.toFloat();
    }
    else if (m_lastKeyName == "wind_deg")
    {
        m_data.m_currentData.m_windDeg = value.toFloat();
    }
    // else if (m_lastKeyName == "wind_gust")
    // {
    //     m_data.m_currentData.m_windGust = value.toFloat();
    // }
    else if (m_lastKeyName == "pressure")
    {
        m_data.m_currentData.m_pressure = value.toInt();
    }
    else if (m_lastKeyName == "humidity")
    {
        m_data.m_currentData.m_humidity = value.toInt();
    }
    // else if (m_lastKeyName == "dew_point")
    // {
    //     m_data.m_currentData.m_dew_point = value.toFloat();
    // }
    else if (m_lastKeyName == "uvi")
    {
        m_data.m_currentData.m_uvi = value.toFloat();
    }
}

void OneCallListener::ReadCurrentWeather(const String& value)
{
    if (m_lastKeyName == "id")
    {
        m_data.m_currentData.m_weatherId = value.toInt();
    }
    else if (m_lastKeyName == "main")
    {
        m_data.m_currentData.m_weatherMain = value;
    }
    else if (m_lastKeyName == "description")
    {
        m_data.m_currentData.m_weatherDescription = value;
    }
    else if (m_lastKeyName == "icon")
    {
        memcpy(m_data.m_currentData.m_weatherIcon, (void*)value.c_str(), sizeof(m_data.m_currentData.m_weatherIcon));
    }
}

void OneCallListener::ReadHourly(const String& value)
{
    if (m_lastKeyName == "dt")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_dateTime = value.toInt();
    }
    else if (m_lastKeyName == "temp")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_temparature = value.toFloat();
    }
    else if (m_lastKeyName == "feels_like")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_feels_like = value.toFloat();
    }
    else if (m_lastKeyName == "humidity")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_humidity = value.toInt();
    }
    else if (m_lastKeyName == "uvi")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_uvi = value.toFloat();
    }
    else if (m_lastKeyName == "wind_speed")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_windSpeed = value.toFloat();
    }
    else if (m_lastKeyName == "wind_deg")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_windDegree = value.toInt();
    }
}

void OneCallListener::ReadHourlyWeater(const String& value)
{
    if (m_lastKeyName == "description")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_description = value;
    }
    else if (m_lastKeyName == "icon")
    {
        memcpy(m_data.m_hourlyData[m_data.m_hourlyCount].m_icon, (void*)value.c_str(), sizeof(m_data.m_hourlyData[m_data.m_hourlyCount].m_icon));
    }
}

void OneCallListener::ReadHourlyRain(const String& value)
{
    if (m_lastKeyName == "1h")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_rain1h = value.toInt();
    }
    else if (m_lastKeyName == "3h")
    {
        m_data.m_hourlyData[m_data.m_hourlyCount].m_rain3h = value.toInt();
    }
}

void OneCallListener::ReadHourlySnow(const String& value)
{
    if (m_lastKeyName == "1h")
    {
        m_data.m_hourlyData[m_arrayDepth].m_snow1h = value.toInt();
    }
    else if (m_lastKeyName == "3h")
    {
        m_data.m_hourlyData[m_arrayDepth].m_snow3h = value.toInt();
    }
}

void OneCallListener::ReadDaily(const String& value)
{
    if (m_lastKeyName == "dt")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_dateTime = value.toInt();
        m_data.m_dailyData[m_data.m_dailyCount].m_dayOfWeek = CalcDayOfWeek(value.toInt());
    }
    else if (m_lastKeyName == "sunrise")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_sunRise = value.toInt();
    }
    else if (m_lastKeyName == "sunset")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_sunSet = value.toInt();
    }
    else if (m_lastKeyName == "moonrise")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_moonRise = value.toInt();
    }
    else if (m_lastKeyName == "moonset")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_moonSet = value.toInt();
    }
    else if (m_lastKeyName == "moon_phase")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_moonPhase = value.toFloat();
    }
    else if (m_lastKeyName == "pressure")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_pressure = value.toInt();
    }
    else if (m_lastKeyName == "humidity")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_humidity = (char)value.toInt();
    }
    else if (m_lastKeyName == "dew_point")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_dewPoint = value.toFloat();
    }
    else if (m_lastKeyName == "wind_speed")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_windSpeed = value.toFloat();
    }
    else if (m_lastKeyName == "wind_deg")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_windDeg = value.toFloat();
    }
    else if (m_lastKeyName == "wind_gust")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_windGust = value.toFloat();
    }
    else if (m_lastKeyName == "clouds")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_cloudsInPercent = (char)value.toInt();
    }
    else if (m_lastKeyName == "pop")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_probabilityOfPrecipitation = value.toFloat();
    }
    else if (m_lastKeyName == "rain")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_rainMM = value.toFloat();
    }
    else if (m_lastKeyName == "snow")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_snowMM = value.toFloat();
    }
    else if (m_lastKeyName == "uvi")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_uvi = value.toFloat();
    }
    else
    {
        int x = 0;
        x++;
    }
    //else if (m_lastKeyName.c_str() == "summary")
    //{
    //    m_data.m_dailyData[m_data.m_dailyCount].m_summaryInEnglish = value;
    //}
}

void OneCallListener::ReadDailyTemp(const String& value)
{
    if (m_lastKeyName == "day")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_tempDay = value.toFloat();
    }
    else if (m_lastKeyName == "min")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_tempMin = value.toFloat();
    }
    else if (m_lastKeyName == "max")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_tempMax = value.toFloat();
    }
    else if (m_lastKeyName == "eve")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_tempEve = value.toFloat();
    }
    else if (m_lastKeyName == "night")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_tempNight = value.toFloat();
    }
    else if (m_lastKeyName == "morn")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_tempMorn = value.toFloat();
    }
}

void OneCallListener::ReadDailyWeather(const String& value)
{
    if (m_lastKeyName == "id")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_weatherId = value.toInt();
    }
    else if (m_lastKeyName == "main")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_weatherMain = value;
    }
    else if (m_lastKeyName == "description")
    {
        m_data.m_dailyData[m_data.m_dailyCount].m_weatherDescription = value;
    }
    else if (m_lastKeyName == "icon")
    {
        memcpy(m_data.m_dailyData[m_data.m_dailyCount].m_weatherIcon, (void*)value.c_str(), sizeof(m_data.m_dailyData[m_data.m_dailyCount].m_weatherIcon));
    }
    
}