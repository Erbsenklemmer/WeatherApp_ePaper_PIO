#pragma once

#include <JsonListener.h>

#include "OneCallData.h"

#define m_stackSize 5

class OneCallListener: public JsonListener {

  public:
    OneCallListener();
    virtual ~OneCallListener();
  
    virtual void whitespace(char c);
    virtual void startDocument();
    virtual void key(String key);
    virtual void value(String value);
    virtual void startArray();
    virtual void endArray();
    virtual void startObject();
    virtual void endObject();
    virtual void endDocument();

    void Reset();

    void PushObjectName(const String& name);
    const String PopObjectName();

    const int16_t CalcDayOfWeek(int unixTimeRaw);

    OneCallData m_data;

private:

    int m_indent;
    
    String m_lastKeyName;
    String m_lastObjectName;

    String m_stackObjectName[m_stackSize];
    void DumpStackObjectName();

    unsigned char m_objectDepth;
    unsigned char m_arrayDepth;

    String addIndent;
    String strIndent;
   
    bool dump;

private:
    void ReadCurrent(const String& value);
    void ReadCurrentWeather(const String& value);

    void ReadHourly(const String& value);
    void ReadHourlyWeater(const String& value);
    void ReadHourlyRain(const String& value);
    void ReadHourlySnow(const String& value);

    void ReadDaily(const String& value);
    void ReadDailyTemp(const String& value);
    void ReadDailyWeather(const String& value);
};
