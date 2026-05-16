#include <Arduino.h>

#include <UnixTime.h>
#include <JsonStreamingParser.h>

#include "WifiParser.h"
#include "OneCallListener.h"
#include "EPaperWeatherDrawer.h"

int go=1;
#define oneSecond 1e6
uint64_t sleepDuration = 60 * oneSecond;

void setup() 
{
  Serial.begin(115200);
  while (Serial.availableForWrite() <= 0)
  {}

  Serial.println("setup - start");

  WifiParser::setup();

  EPaperWeatherDrawer::setup();

//   Serial.println("setup - done");
// }

// void loop() 
// {
//   Serial.println("Starting LOOP");

  OneCallListener oneCallListener;
  oneCallListener.Reset();//needed if we want to loop

  JsonStreamingParser parser;
  parser.setListener(&oneCallListener);

  bool weatherDataAvailable = true;
#ifdef __Test_Paint_DailyData__
   //memcpy(m_data.m_dailyData[m_data.m_dailyCount].m_weatherIcon, (void*)value.c_str(), sizeof(m_data.m_dailyData[m_data.m_dailyCount].m_weatherIcon));
  switch (go)
  { 
    case 0:
      // memcpy(oneCallListener.m_data.m_dailyData[0].m_weatherIcon, "01d", sizeof(oneCallListener.m_data.m_dailyData[0].m_weatherIcon));
      // memcpy(oneCallListener.m_data.m_dailyData[1].m_weatherIcon, "01n", sizeof(oneCallListener.m_data.m_dailyData[1].m_weatherIcon));
      // memcpy(oneCallListener.m_data.m_dailyData[2].m_weatherIcon, "02d", sizeof(oneCallListener.m_data.m_dailyData[2].m_weatherIcon));
      break;
    case 1:
      memcpy(oneCallListener.m_data.m_dailyData[0].m_weatherIcon, "02n", sizeof(oneCallListener.m_data.m_dailyData[0].m_weatherIcon));
      oneCallListener.m_data.m_dailyData[0].m_tempDay = 10.26;
      oneCallListener.m_data.m_dailyData[0].m_tempMin = -10.26;
      oneCallListener.m_data.m_dailyData[0].m_tempMax = 88.88;
      oneCallListener.m_data.m_dailyData[0].m_weatherDescription = "Überwiegend bewölkt °C";
      // memcpy(oneCallListener.m_data.m_dailyData[1].m_weatherIcon, "03d", sizeof(oneCallListener.m_data.m_dailyData[1].m_weatherIcon));
      // memcpy(oneCallListener.m_data.m_dailyData[2].m_weatherIcon, "03n", sizeof(oneCallListener.m_data.m_dailyData[2].m_weatherIcon));
      break;
    case 2:
      // memcpy(oneCallListener.m_data.m_dailyData[0].m_weatherIcon, "04d", sizeof(oneCallListener.m_data.m_dailyData[0].m_weatherIcon));
      // memcpy(oneCallListener.m_data.m_dailyData[1].m_weatherIcon, "09d", sizeof(oneCallListener.m_data.m_dailyData[1].m_weatherIcon));
      // memcpy(oneCallListener.m_data.m_dailyData[2].m_weatherIcon, "09n", sizeof(oneCallListener.m_data.m_dailyData[2].m_weatherIcon));
      break;
    case 3:
      // memcpy(oneCallListener.m_data.m_dailyData[0].m_weatherIcon, "10d", sizeof(oneCallListener.m_data.m_dailyData[0].m_weatherIcon));
      // memcpy(oneCallListener.m_data.m_dailyData[1].m_weatherIcon, "13d", sizeof(oneCallListener.m_data.m_dailyData[1].m_weatherIcon));
      // memcpy(oneCallListener.m_data.m_dailyData[2].m_weatherIcon, "50d", sizeof(oneCallListener.m_data.m_dailyData[2].m_weatherIcon));
      break;
  }

  if (go >= 3)
    go = 1;
  // else 
  //   ++go;

#else
  weatherDataAvailable = WifiParser::parse(&parser);
#endif //__Test_Paint_DailyData__

  if (weatherDataAvailable)
  {
    UnixTime unixTime(oneCallListener.m_data.m_timezone_offset_in_half_hours / 2);
    EPaperWeatherDrawer::drawOneCallData(oneCallListener.m_data, unixTime);
  }
  else
  {
    Serial.println("Failed to get any weather data");
  }
  
  // UnixTime unixTime(oneCallListener.m_data.m_timezone_offset_in_half_hours / 2);
  // String offset = String(oneCallListener.m_data.m_timezone_offset_in_half_hours / 2);
  // Serial.println("offset: " + offset);
  // //CurrentData *pCurrentData = &oneCallListener.m_data.m_currentData;
  
  // //unixTime.getDateTime(pCurrentData->m_dateTime);
  // unixTime.getDateTime(oneCallListener.m_data.m_currentData.m_dateTime);

  // String dateTimeString = "Current time:  " +
  //   String(unixTime.hour) + ":" + String(unixTime.minute) + ":" + String(unixTime.second) + " " + 
  //   String(unixTime.day) + "." + unixTime.month/*monthNames[unixTime.month-1]*/ + "." + String(unixTime.year);
  // Serial.println(dateTimeString);

  // Serial.println("Waiting");
  // delay(60 * 1000);


  //ESP.deepSleep(sleepDuration);
}

void loop() 
{
}