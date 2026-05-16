#include "EPaperWeatherDrawer.h"

#include "windy_arrows_font.c"

GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT> display(GxEPD2_420c(D8, D3, D1, D2));
//GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> display(GxEPD2_420c_Z21(D8, D3, D1, D2));

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

const  int COLOR_FOREGROUND = GxEPD_BLACK;
const  int COLOR_BACKGROUND = GxEPD_WHITE;
const  int COLOR_RED = GxEPD_RED;

const char* weekdays[] = { "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag" };
const char* monthNames[] = { "Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember" };

void EPaperWeatherDrawer::setup()
{
  Serial.println("EPaperWeatherDrawer::setup() enter");

  display.init();

  u8g2Fonts.begin(display);  
  u8g2Fonts.setForegroundColor(COLOR_FOREGROUND);
  u8g2Fonts.setBackgroundColor(COLOR_BACKGROUND);

  display.setRotation(1);
  display.setFullWindow();

  //display.setUTF8Print(true);

  Serial.println("EPaperWeatherDrawer::setup() leaving");
}

void EPaperWeatherDrawer::drawOneCallData(const OneCallData& oneCallData, UnixTime unixTime)
{
  // Serial.println("Drawing disabled!!!");
  // return;

  display.firstPage();
  Serial.println(String("GxEPD2_420c::WIDTH: ") + GxEPD2_420c::WIDTH + String(", GxEPD2_420c::HEIGHT: ") + GxEPD2_420c::HEIGHT);

  do 
  {
    display.fillScreen(COLOR_BACKGROUND);

    drawTodayData(0, 0, unixTime, oneCallData.m_currentData, oneCallData.m_dailyData[0]);
    drawForcasts(200,0, unixTime, oneCallData);
    
    drawHumidityPressureUVIndex(0, 0, unixTime, oneCallData.m_dailyData[0]);

    drawDailyData(  0, 210, oneCallData.m_dailyData[1]);
    drawDailyData(100, 210, oneCallData.m_dailyData[2]);
    drawDailyData(200, 210, oneCallData.m_dailyData[3]);

    // display.drawLine(100, 100, 100, 400, COLOR_FOREGROUND);
    // display.drawLine(200, 100, 200, 400, COLOR_RED);
    
    // display.drawLine(0, 100, 300, 100, COLOR_RED);
    display.drawLine(0, 200, 300, 200, COLOR_RED);
    //display.drawLine(0, 300, 300, 300, COLOR_RED);
  } 
  while(display.nextPage());

  display.hibernate();
}

void EPaperWeatherDrawer::drawTodayData(int x, int y, UnixTime unixTime, const CurrentData& currentData, const DailyData& todayData)
{
  String textOut;
  int16_t textWidth, textHeight;
  int16_t startX = x; 
  int16_t startYNext = y;

  u8g2Fonts.setFont(u8g2_font_helvR14_tf);
  textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();
  startYNext += textHeight;

  unixTime.getDateTime(currentData.m_dateTime);

//day
  textOut = String(weekdays[todayData.m_dayOfWeek]) + ", " + String(unixTime.day) + ". " + String(monthNames[unixTime.month-1]);

  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

  u8g2Fonts.setCursor(startX + ((display.width()-textWidth) / 2), startYNext);
  u8g2Fonts.print(textOut);

//time
  String fill = unixTime.minute < 10 ? ":0" : ":";
  textOut = String(unixTime.hour) + fill + String(unixTime.minute);

  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

  //u8g2Fonts.setCursor(startX + ((display.width()-textWidth)) - 5, startYNext);//rechts bündig
  u8g2Fonts.setCursor(startX + 5, startYNext);//links bündig
  u8g2Fonts.print(textOut);

//icon and temp in the middle
//draw Icon
  //startY += 70;
  startX = 100;
  //startYNext += textHeight;

  // DrawIcon(0, startYNext, "10d");
  // display.drawRect(0, startYNext, 100, 100, COLOR_RED);

  // DrawIcon(100, startYNext, "50d");
  // display.drawRect(100, startYNext, 100, 100, COLOR_RED);

  // DrawIcon(200, startYNext, "11n");
  // display.drawRect(200, startYNext, 100, 100, COLOR_RED);

  DrawIcon(startX, startYNext, currentData.m_weatherIcon);

  startYNext += 100;

// main temp
  u8g2Fonts.setFont(u8g2_font_helvB18_tf);
  textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();
  startYNext += textHeight;

  textOut = String(todayData.m_tempDay, 1) + "°";
  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());
//  Serial.println(textOut + " length: " + String(textWidth));
  
  u8g2Fonts.setCursor(startX + ((100-textWidth) / 2), startYNext);
  u8g2Fonts.print(textOut);

// // min / max temp
  u8g2Fonts.setFont(u8g2_font_helvR14_tf);

  textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();
  startYNext += textHeight;

  textOut = String(todayData.m_tempMax, 0) + "° | " + String(todayData.m_tempMin, 0) + "°";
  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());
//  Serial.println(textOut + " length: " + String(textWidth));

  u8g2Fonts.setCursor(startX + ((100-textWidth) / 2), startYNext);
  u8g2Fonts.print(textOut);
}

void EPaperWeatherDrawer::drawForcasts(int x, int y, UnixTime unixTime, const OneCallData& oneCallData)
{
  String textOut;
  int16_t textWidth, textHeight;
  int16_t startX = x; 
  int16_t startYNext = y; 

  u8g2Fonts.setFont(u8g2_font_helvR14_tf);
  textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();
  startYNext += textHeight;

//  Serial.println("++++++++++  starty: " + String(textHeight));

  int slotHeight = 55;
  
  // u8g2Fonts.setFont(u8g2_font_helvR12_tf);
  // textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();

  int timeWidth = u8g2Fonts.getUTF8Width("XX:XX");

  for (int i=0; i < hourlyForeCasts; i++)
  {
    DrawIcon_Small(startX + 55, startYNext+5, oneCallData.m_hourlyData[i].m_icon);
    // if (i == 0)
    //   DrawIcon_Small(startX + 55, startYNext+5, "09d");
    // else if (i == 1)
    //   DrawIcon_Small(startX + 55, startYNext+5, "10n");
    // else if (i == 2)
    //   DrawIcon_Small(startX + 55, startYNext+5, "10d");

    unixTime.getDateTime(oneCallData.m_hourlyData[i].m_dateTime);
    // Serial.println(unixTime.hour);
    u8g2Fonts.setCursor(startX, startYNext);
    Serial.println("height: " + String(textHeight));

    textOut = String(unixTime.hour) + ":";
    if (unixTime.minute < 10)
      textOut += "0";
    textOut += String(unixTime.minute);

    textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

    u8g2Fonts.setCursor(startX + timeWidth - textWidth, startYNext + textHeight + 10);
    u8g2Fonts.print(textOut);

    textOut = String(oneCallData.m_hourlyData[i].m_temparature, 0) + "°";
    textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

    u8g2Fonts.setCursor(startX + timeWidth - textWidth, startYNext + 2* textHeight + 10);
    u8g2Fonts.print(textOut);

    startYNext += slotHeight;
  }
}

void EPaperWeatherDrawer::drawHumidityPressureUVIndex(int x, int y, UnixTime unixTime, const DailyData& dailyData)
{
  String textOut;
  int16_t textWidth, textHeight;
  int16_t startX = x; 
  int16_t startYNext = y; 

  u8g2Fonts.setFont(u8g2_font_helvR14_tf);
  textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();
  startYNext += 2*textHeight;

  textOut = "XXXX";// hPa";
  int textWidthDigits = u8g2Fonts.getUTF8Width(textOut.c_str());

  textHeight += 2;

  // //rain/snow
  // startYNext += textHeight;

  // textOut = String("Regen: ");
  // if(dailyData.m_snowMM > 0)
  //   textOut = String("Schnee: ");
  // textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

  // u8g2Fonts.setCursor(startX + textWidthDigits - textWidth, startYNext);
  // u8g2Fonts.print(textOut);

  // u8g2Fonts.setCursor(startX + textWidthDigits, startYNext);
  // if(dailyData.m_snowMM > 0)
  //   textOut = String(dailyData.m_snowMM, 0) + " mm";
  // else if(dailyData.m_rainMM > 0)
  //   textOut = String(dailyData.m_rainMM, 0) + " mm";
  // else
  //   u8g2Fonts.print(" -");
  
  //humidity
  startYNext += textHeight;
  textOut = String((int)dailyData.m_humidity);
  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

  u8g2Fonts.setCursor(startX + textWidthDigits - textWidth, startYNext);
  u8g2Fonts.print(textOut);

  u8g2Fonts.setCursor(startX + textWidthDigits, startYNext);
  u8g2Fonts.print(" %");

   //pressure
  startYNext += textHeight;
  textOut = String(dailyData.m_pressure);
  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

  u8g2Fonts.setCursor(startX + textWidthDigits - textWidth, startYNext);
  u8g2Fonts.print(textOut);

  u8g2Fonts.setCursor(startX + textWidthDigits, startYNext);
  u8g2Fonts.print(" hPa");

  //uvi
  startYNext += textHeight;
  textOut = "uvi";
  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

  u8g2Fonts.setCursor(startX + textWidthDigits - textWidth, startYNext);
  u8g2Fonts.print(textOut);

  u8g2Fonts.setCursor(startX + textWidthDigits, startYNext);
  u8g2Fonts.print(" " + String(dailyData.m_uvi, 1));

  //wind
  startYNext += textHeight;
  textOut = "wind";
  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

  u8g2Fonts.setCursor(startX + textWidthDigits - textWidth, startYNext);
  u8g2Fonts.print(textOut);

  u8g2Fonts.setCursor(startX + textWidthDigits, startYNext);
  u8g2Fonts.print(String(dailyData.m_windSpeed / 3.6, 0));// + "km/h ");
  
  u8g2Fonts.setFont(u8g2_font_helvB08_tf);
  u8g2Fonts.print("km/h");

  //wind direction
  float help = dailyData.m_windDeg / 45.;
  int windyIcon = int(help + 0.5);
  if (windyIcon >= 8)
    windyIcon -= 8;

  Serial.println("WindSpeed: " + String(dailyData.m_windSpeed) +  ", windyIcon: " + String(windyIcon) + ", deg: " + String(dailyData.m_windDeg));

  u8g2Fonts.setFont(windy_arrows_font);
  u8g2Fonts.setCursor(u8g2Fonts.getCursorX(), u8g2Fonts.getCursorY() - 4);

  u8g2Fonts.write(windyIcon + 32);

  u8g2Fonts.setCursor(u8g2Fonts.getCursorX(), u8g2Fonts.getCursorY() + 4);
  u8g2Fonts.setFont(u8g2_font_helvR14_tf);

//sun
  startYNext += textHeight;
  u8g2Fonts.setCursor(startX, startYNext);

  textWidth = u8g2Fonts.getUTF8Width(" ");
  u8g2Fonts.setCursor(startX + textWidth, startYNext);

  u8g2Fonts.setFont(u8g2_font_unifont_t_weather);
  u8g2Fonts.write(51);//sunny icon
  u8g2Fonts.setFont(u8g2_font_helvR14_tf);

  int curX = u8g2Fonts.getCursorX();
  unixTime.getDateTime(dailyData.m_sunRise);
  u8g2Fonts.print("  " + String(unixTime.hour) + ":" + String(unixTime.minute));
  
  startYNext += textHeight;
  u8g2Fonts.setCursor(curX, startYNext);

  unixTime.getDateTime(dailyData.m_sunSet);
  u8g2Fonts.print("  " + String(unixTime.hour) + ":" + String(unixTime.minute));

  //moon
  startYNext += textHeight;
  u8g2Fonts.setFont(windy_arrows_font);

  Serial.println("moon phase: " + String(dailyData.m_moonPhase, 3));
  //0 und 1 sind neu mond 
  //0.25 erstes viertel, 0.5 Vollmond, 0.75 letztes Viertel

  //u8g2Fonts.setCursor(startX, startYNext);
  //u8g2Fonts.write(32);

  //sun
}

void EPaperWeatherDrawer::drawDailyData(int x, int y, const DailyData& dailyData)
{
  int startX = x;
  int startY = y;

  String textOut;
  int16_t textWidth, textHeight;
  int16_t startYNext = startY; 

//weekday
  u8g2Fonts.setFont(u8g2_font_helvR14_tf);

  textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();
  startYNext += textHeight;

  textOut = weekdays[dailyData.m_dayOfWeek];

  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());

  u8g2Fonts.setCursor(startX + ((100-textWidth) / 2), startYNext);
  u8g2Fonts.print(textOut);

//draw Icon
  //startY += 70;
  DrawIcon(startX, startYNext, dailyData.m_weatherIcon);

  startYNext += 100;

// weather description
  // u8g2Fonts.setFont(u8g2_font_helvB14_tf);

  // textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();
  // startYNext += textHeight;

  // u8g2Fonts.setCursor(startX, startYNext);
  // u8g2Fonts.print(dailyData.m_weatherDescription);

// main temp
  u8g2Fonts.setFont(u8g2_font_helvB18_tf);

  textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();
  startYNext += textHeight;

  textOut = String(dailyData.m_tempDay, 1) + "°";
  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());
  Serial.println(textOut + " length: " + String(textWidth));
  
  u8g2Fonts.setCursor(startX + ((100-textWidth) / 2), startYNext);
  u8g2Fonts.print(textOut);

// min / max temp
  u8g2Fonts.setFont(u8g2_font_helvR14_tf);

  textHeight = u8g2Fonts.getFontAscent() - u8g2Fonts.getFontDescent();
  startYNext += textHeight;

  textOut = String(dailyData.m_tempMax, 0) + "° | " + String(dailyData.m_tempMin, 0) + "°";
  textWidth = u8g2Fonts.getUTF8Width(textOut.c_str());
  Serial.println(textOut + " length: " + String(textWidth));

  u8g2Fonts.setCursor(startX + ((100-textWidth) / 2), startYNext);
  u8g2Fonts.print(textOut);
}

void EPaperWeatherDrawer::DrawIcon(int x, int y, const String& crIcon)
{
  Serial.println("drawing icon: " + crIcon + " at: " + x + ", " + y);

  if (crIcon == "01d")
    DrawSun(x, y);//01d;
  else if (crIcon == "01n")    
    DrawMoon(x, y);//01n;

  else if (crIcon == "02d")
    DrawMediumSunWithCloud(x, y);//02d
  else if (crIcon == "02n")
    DrawMediumMoonWithCloud(x, y);//02n

  else if (crIcon == "03d")
    DrawBlackAndWhiteCloudWithSun(x, y);//03d  
  else if (crIcon == "03n")
    DrawBlackAndWhiteCloudWithMoon(x, y);//03n

  else if (crIcon == "04d" || crIcon == "04n")
    DrawBlackAndWhiteCloud(x, y);//04d/n

  else if (crIcon == "09d" || crIcon == "09n")
    DrawRain(x, y);//10d  

  else if (crIcon == "10d")
    DrawMediumSunWithCloudAndRain(x, y);//10d  
  else if (crIcon == "10n")
    DrawMediumMoonWithCloudAndRain(x, y);//10n

  else if (crIcon == "11d" || crIcon == "11n")
    DrawThunderstorm(x, y);//11d

  else if (crIcon == "13d" || crIcon == "13n")
    DrawSnow(x, y);//13d

  else if (crIcon == "50d" || crIcon == "50n")
    DrawFogg(x, y);//50d/n

  else {
    u8g2Fonts.setFont(u8g2_font_helvR14_tf);
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.print(crIcon);
  }
}

void EPaperWeatherDrawer::DrawIcon_Small(int x, int y, const String& crIcon)
{
  Serial.println("drawing small icon: " + crIcon + " at: " + x + ", " + y);

  if (crIcon == "01d")
    DrawSun_Small(x, y);//01d;
  else if (crIcon == "01n")    
    DrawMoon_Small(x, y);//01n;

  else if (crIcon == "02d")
    DrawMediumSunWithCloud_Small(x, y);//02d
  else if (crIcon == "02n")
    DrawMediumMoonWithCloud_Small(x, y);//02n

  else if (crIcon == "03d")
    DrawBlackAndWhiteCloudWithSun_Small(x, y);//03d  
  else if (crIcon == "03n")
    DrawBlackAndWhiteCloudWithMoon_Small(x, y);//03n

  else if (crIcon == "04d" || crIcon == "04n")
    DrawBlackAndWhiteCloud_Small(x, y);//04d/n

  else if (crIcon == "09d" || crIcon == "09n")
    DrawRain_Small(x, y);//10d  

  else if (crIcon == "10d")
    DrawMediumSunWithCloudAndRain_Small(x, y);//10d  
  else if (crIcon == "10n")
    DrawMediumMoonWithCloudAndRain_Small(x, y);//10n

  else if (crIcon == "11d" || crIcon == "11n")
    DrawThunderstorm_Small(x, y);//11d

  else if (crIcon == "13d" || crIcon == "13n")
    DrawSnow_Small(x, y);//13d

  else if (crIcon == "50d" || crIcon == "50n")
    DrawFogg_Small(x, y);//50d/n
  
  else {
    u8g2Fonts.setFont(u8g2_font_helvR14_tf);
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.print(crIcon);
  }
}

void EPaperWeatherDrawer::DrawSun(int offsetX, int offsetY) 
{
  int middleX = 50 + offsetX;  
  int middleY = 50 + offsetY;

  int length = 70;

  display.drawLine(middleX-length/2, middleY, middleX+length/2, middleY, COLOR_FOREGROUND);//horz
  display.drawLine(middleX, middleY-length/2, middleX, middleY+length/2, COLOR_FOREGROUND);//vert

  for(int angle = 30; angle < 180; angle+=30) 
  {
    if (angle == 90)
      continue;
    
    DrawPolarRay(middleX, middleY, length, angle);
  }

  int radiusSun = 20;
  
  display.fillCircle(middleX, middleY, radiusSun, COLOR_RED);
  display.drawCircle(middleX, middleY, radiusSun, COLOR_FOREGROUND);
}

void EPaperWeatherDrawer::DrawSun_Small(int offsetX, int offsetY) {
  int middleX = 25 + offsetX;  
  int middleY = 25 + offsetY;

  int length = 35;

  display.drawLine(middleX-length/2, middleY, middleX+length/2, middleY, COLOR_FOREGROUND);//horz
  display.drawLine(middleX, middleY-length/2, middleX, middleY+length/2, COLOR_FOREGROUND);//vert

  for(int angle = 30; angle < 180; angle+=30) {
    if (angle == 90)
      continue;
      
    DrawPolarRay(middleX, middleY, length, angle);
  }

  int radiusSun = 10;
  
  display.fillCircle(middleX, middleY, radiusSun, COLOR_RED);
  display.drawCircle(middleX, middleY, radiusSun, COLOR_FOREGROUND);
}

void EPaperWeatherDrawer::DrawMoon(int offsetX, int offsetY) 
{
  offsetX+=3;
  
  const int radiusMoon = 24;
  
  display.fillCircle(45+offsetX, 45+offsetY, radiusMoon, COLOR_RED);
  display.drawCircle(45+offsetX, 45+offsetY, radiusMoon, COLOR_FOREGROUND);
  
  const int offset = 12;
  display.fillCircle(45+offsetX-offset, 45+offsetY-offset, radiusMoon, COLOR_BACKGROUND);
  display.drawCircle(45+offsetX-offset, 45+offsetY-offset, radiusMoon, COLOR_FOREGROUND);
  
  // display.drawCircle(45+offsetX-offset-radiusMoon,        45+offsetY-offset-radiusMoon, 1, COLOR_RED);
  // display.drawCircle(45+offsetX-offset+radiusMoon+offset, 45+offsetY-offset-radiusMoon, 1, COLOR_RED);
  // display.drawCircle(45+offsetX-offset-radiusMoon,        45+offsetY-offset+radiusMoon+offset, 1, COLOR_RED);

  display.fillTriangle(45+offsetX-offset-radiusMoon,        45+offsetY-offset-radiusMoon, 
                       45+offsetX-offset+radiusMoon+offset, 45+offsetY-offset-radiusMoon, 
                       45+offsetX-offset-radiusMoon,        45+offsetY-offset+radiusMoon+offset, COLOR_BACKGROUND);
}

void EPaperWeatherDrawer::DrawMoon_Small(int offsetX, int offsetY) {
  offsetX+=1;

  offsetX+=22;
  offsetY+=22;
  
  const int radiusMoon = 12;
  
  display.fillCircle(offsetX, offsetY, radiusMoon, COLOR_RED);
  display.drawCircle(offsetX, offsetY, radiusMoon, COLOR_FOREGROUND);
  
  const int offset = 6;
  display.fillCircle(offsetX-offset, offsetY-offset, radiusMoon, COLOR_BACKGROUND);
  display.drawCircle(offsetX-offset, offsetY-offset, radiusMoon, COLOR_FOREGROUND);
  
  // display.drawCircle(offsetX-offset-radiusMoon,        offsetY-offset-radiusMoon, 1, COLOR_RED);
  // display.drawCircle(offsetX-offset+radiusMoon+offset, offsetY-offset-radiusMoon, 1, COLOR_RED);
  // display.drawCircle(offsetX-offset-radiusMoon,        offsetY-offset+radiusMoon+offset, 1, COLOR_RED);

  display.fillTriangle(offsetX-offset-radiusMoon,        offsetY-offset-radiusMoon, 
                       offsetX-offset+radiusMoon+offset, offsetY-offset-radiusMoon, 
                       offsetX-offset-radiusMoon,        offsetY-offset+radiusMoon+offset, COLOR_BACKGROUND);
}

void EPaperWeatherDrawer::DrawMediumSunWithCloud(int offsetX, int offsetY) 
{
  offsetX+=3;
  offsetY+=1;
  
  DrawMediumSun(12 + offsetX, 18 + offsetY);
  display.fillRect(offsetX+35, offsetY+70, 30, 10, COLOR_BACKGROUND);
  display.fillRect(offsetX+55, offsetY+45, 25, 10, COLOR_BACKGROUND);

  DrawWhiteCloud(12 + offsetX, 40 + offsetY);
}
void EPaperWeatherDrawer::DrawMediumSunWithCloud_Small(int offsetX, int offsetY) //02d
{
    DrawMediumSun_Small(offsetX+8, offsetY+7);
    DrawWhiteCloud_Small(offsetX+10, offsetY+19);
}

void EPaperWeatherDrawer::DrawMediumMoonWithCloud(int offsetX, int offsetY) 
{
  offsetY+=6;
  DrawMediumMoon(-10 + offsetX, -10 + offsetY);
  DrawWhiteCloud(12  + offsetX, 37 + offsetY);
}  
void EPaperWeatherDrawer::DrawMediumMoonWithCloud_Small(int offsetX, int offsetY) //02n
{
  DrawMediumMoon_Small(offsetX-10, offsetY-11);
  DrawWhiteCloud_Small(offsetX+10, offsetY+19);
}  

void EPaperWeatherDrawer::DrawBlackAndWhiteCloudWithSun(int offsetX, int offsetY) 
{
  offsetX+=3;
  offsetY+=5;
  Serial.println("DrawBlackAndWhiteCloudWithSun at: " + String(offsetX) + ", " + offsetY);
  DrawSmallSun(offsetX+10, offsetY+9);
  DrawBlackAndWhiteCloud(offsetX, offsetY);
}
void EPaperWeatherDrawer::DrawBlackAndWhiteCloudWithSun_Small(int offsetX, int offsetY) //03d
{
  DrawSmallSun_Small(offsetX+5, offsetY+7);
  DrawBlackAndWhiteCloud_Small(offsetX-2, offsetY+5);
}

void EPaperWeatherDrawer::DrawBlackAndWhiteCloudWithMoon(int offsetX, int offsetY) 
{
    Serial.println("DrawBlackAndWhiteCloudWithMoon at: " + String(offsetX) + ", " + offsetY);
    DrawSmallMoon(offsetX-15, offsetY-7);
    DrawBlackAndWhiteCloud(offsetX, offsetY+7);
}
void EPaperWeatherDrawer::DrawBlackAndWhiteCloudWithMoon_Small(int offsetX, int offsetY) //03n
{
    DrawSmallMoon_Small(offsetX-29, offsetY-29);
    DrawBlackAndWhiteCloud_Small(offsetX-2, offsetY+5);
}

void EPaperWeatherDrawer::DrawBlackAndWhiteCloud(int offsetX, int offsetY) //04d/n
{
  offsetX += 7;
  offsetY += 25;
  DrawWhiteCloud(offsetX+10, offsetY);
  DrawBlackCloud(offsetX   , offsetY+10);
}
void EPaperWeatherDrawer::DrawBlackAndWhiteCloud_Small(int offsetX, int offsetY) //04d/n
{
  offsetX+=3;
  offsetY+=10;
  DrawWhiteCloud_Small(offsetX+10, offsetY);
  DrawBlackCloud_Small(offsetX+3 , offsetY+5);
}

void EPaperWeatherDrawer::DrawMediumSunWithCloudAndRain(int offsetX, int offsetY)//10d
{
  DrawMediumSunWithCloud(offsetX-3, offsetY-5);
    
  for(int i = 0; i <= 50; i+=8) 
  {
    display.drawLine(i+24+offsetX, 70+offsetY, i+27+offsetX, 77+offsetY, COLOR_FOREGROUND);
  }
}
void EPaperWeatherDrawer::DrawMediumSunWithCloudAndRain_Small(int offsetX, int offsetY) //10d
{
  DrawMediumSunWithCloud_Small(offsetX, offsetY);

  for(int i = 0; i <= 20; i+=4) {
    display.drawLine(i+15+offsetX, 36+offsetY, i+17+offsetX, 39+offsetY, COLOR_FOREGROUND);
  } 
}

void EPaperWeatherDrawer::DrawMediumMoonWithCloudAndRain(int offsetX, int offsetY)//10n
{
  DrawMediumMoonWithCloud(offsetX-3, offsetY-5);
    
  for(int i = 0; i <= 50; i+=8) 
  {
    display.drawLine(i+24+offsetX, 70+offsetY, i+27+offsetX, 77+offsetY, COLOR_FOREGROUND);
  }
}
void EPaperWeatherDrawer::DrawMediumMoonWithCloudAndRain_Small(int offsetX, int offsetY) //10n
{
  DrawMediumMoonWithCloud_Small(offsetX, offsetY);
    
  for(int i = 0; i <= 20; i+=4) {
    display.drawLine(i+15+offsetX, 36+offsetY, i+17+offsetX, 39+offsetY, COLOR_FOREGROUND);
  } 
}

void EPaperWeatherDrawer::DrawLine(PointData p1, PointData p2, uint16_t color) {
  display.drawLine(p1.x, p1.y, p2.x, p2.y, color);
}

void EPaperWeatherDrawer::FillTriangle(PointData p1, PointData p2, PointData p3, uint16_t color) {
  display.fillTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color);
}

void EPaperWeatherDrawer::DrawThunderstorm(int offsetX, int offsetY) {
  offsetY-=12;

  DrawRain(offsetX, offsetY);

  int x = offsetX+5;
  int y = offsetY+46;

  PointData p1(44+x, 24+y);
  PointData p2(35+x, 24+y);
  PointData p3(24+x, 40+y);
  PointData p4(30+x, 40+y);
  PointData p5(22+x, 55+y);
  PointData p6(40+x, 35+y);
  PointData p7(36+x, 35+y);

  FillTriangle(p1, p2, p3, COLOR_RED);
  FillTriangle(p3, p1, p7, COLOR_RED);
  FillTriangle(p3, p4, p7, COLOR_RED);
  FillTriangle(p4, p6, p7, COLOR_RED);
  FillTriangle(p4, p6, p5, COLOR_RED);

  DrawLine(p1, p2, COLOR_FOREGROUND);
  DrawLine(p2, p3, COLOR_FOREGROUND);
  DrawLine(p3, p4, COLOR_FOREGROUND);
  DrawLine(p4, p5, COLOR_FOREGROUND);
  DrawLine(p5, p6, COLOR_FOREGROUND);
  DrawLine(p6, p7, COLOR_FOREGROUND);
  DrawLine(p7, p1, COLOR_FOREGROUND);
}
void EPaperWeatherDrawer::DrawThunderstorm_Small(int offsetX, int offsetY) //11d/n
{
  DrawRain_Small(offsetX-2, offsetY-5);

  offsetX+=1;
  offsetY+=20;

  int x = offsetX * 2;//duplicate, will be halfend below
  int y = offsetY * 2;

//  1 ----------7
//  /          /
// 2 -- 3  5--6
//   /    /
//  /  /
//  4

  PointData p1((44+x)/2, (24+y)/2);
  PointData p2((/*36*/32+x)/2, (24+y)/2);
  PointData p3((24+x)/2, (40+y)/2);
  PointData p4((30+x)/2, (40+y)/2);
  PointData p5((22+x)/2, (55+y)/2);
  PointData p6((40+x)/2, (35+y)/2);
  PointData p7((36+x)/2, (35+y)/2);

  Serial.println("thunder_small p1 x, y: " + String(p1.x) + ", " + String(p1.y));

  FillTriangle(p1, p2, p3, COLOR_RED);
  FillTriangle(p3, p1, p7, COLOR_RED);
  FillTriangle(p3, p4, p7, COLOR_RED);
  FillTriangle(p4, p6, p7, COLOR_RED);
  FillTriangle(p4, p6, p5, COLOR_RED);

  DrawLine(p1, p2, COLOR_FOREGROUND);
  DrawLine(p2, p3, COLOR_FOREGROUND);
  DrawLine(p3, p4, COLOR_FOREGROUND);
  DrawLine(p4, p5, COLOR_FOREGROUND);
  DrawLine(p5, p6, COLOR_FOREGROUND);
  DrawLine(p6, p7, COLOR_FOREGROUND);
  DrawLine(p7, p1, COLOR_FOREGROUND);
  //DrawLine(p8, p1, COLOR_FOREGROUND);
}

void EPaperWeatherDrawer::DrawRain(int offsetX, int offsetY)//10d
{
  offsetX += 4;
  offsetY += 20;

  DrawWhiteCloud(offsetX+20, offsetY);
  DrawBlackCloud(offsetX   , offsetY+24);

  for(int i = 40; i <= 60; i+=8) {
    display.drawLine(i+22+offsetX, 33+offsetY, i+25+offsetX, 40+offsetY, COLOR_FOREGROUND);
  }
  
  for(int i = 0; i <= 50; i+=8) {
      display.drawLine(i+12+offsetX, 57+offsetY, i+15+offsetX, 64+offsetY, COLOR_FOREGROUND);
  }
} 
void EPaperWeatherDrawer::DrawRain_Small(int offsetX, int offsetY) //09n/d
{
  offsetX += 3;
  offsetY += 10;

  DrawWhiteCloud_Small(offsetX+10, offsetY+1);
  DrawBlackCloud_Small(offsetX+ 2, offsetY+9);

  for(int i = 28; i <= 36; i+=4) {
    display.drawLine(i+offsetX, 18+offsetY, i+2+offsetX, 20+offsetY, COLOR_FOREGROUND);
  }
  
  for(int i = 0; i <= 16; i+=4) {
      display.drawLine(i+9+offsetX, 27+offsetY, i+11+offsetX, 29+offsetY, COLOR_FOREGROUND);
  }
}  
void EPaperWeatherDrawer::DrawSnow(int offsetX, int offsetY) //13d 
{
  offsetX += 3;
  offsetY += 10;

  DrawWhiteCloud(offsetX+20, offsetY);
  DrawBlackCloud(offsetX   , offsetY+24);
    
  offsetY+=15;

  for (int j = 0; j < 2; j++)
  {
    int i = 0;
    for (; i <= 40; i+=8) 
    {
      display.drawCircle(i+16+offsetX, 47+offsetY, 1, COLOR_FOREGROUND);
      display.drawCircle(i+20+offsetX, 52+offsetY, 1, COLOR_FOREGROUND);
      display.drawCircle(i+16+offsetX, 57+offsetY, 1, COLOR_FOREGROUND);
    }
    display.drawCircle(i+16+offsetX, 57+offsetY, 1, COLOR_FOREGROUND);

    offsetX+=20;
    offsetY-=24;
  }
}
void EPaperWeatherDrawer::DrawSnow_Small(int offsetX, int offsetY) 
{
  offsetY+=10;

  DrawBlackAndWhiteCloud_Small(offsetX, offsetY-7);
  
  offsetX+=6;
  offsetY+=7;

  int flake = 0;
  for(int i = 0; i <= 20; i+=6, flake++) {
    display.drawCircle(i+7+offsetX, 22+offsetY, 1, COLOR_FOREGROUND);
    if (flake < 3)
      display.drawCircle(i+10+offsetX, 25+offsetY, 1, COLOR_FOREGROUND);
  }
}

void EPaperWeatherDrawer::DrawFogg(int offsetX, int offsetY) //50d
{
  Serial.println("Draw Fog: " + String(offsetX) + ", " + offsetY);
  // display.drawLine(30, 29, 67, 29, COLOR_FOREGROUND);
  // display.drawLine(25, 36, 75, 36, COLOR_FOREGROUND);
  // display.drawLine(15, 43, 70, 43, COLOR_FOREGROUND);
  // display.drawLine(20, 50, 67, 50, COLOR_FOREGROUND);
  // display.drawLine(23, 57, 73, 57, COLOR_FOREGROUND);
  // display.drawLine(24, 64, 72, 64, COLOR_FOREGROUND);
  // display.drawLine(30, 71, 67, 71, COLOR_FOREGROUND);


  display.drawLine(30+offsetX, 36+offsetY, 67+offsetX, 36+offsetY, COLOR_FOREGROUND);
  //display.drawLine(25+offsetX, 36+offsetY, 75+offsetX, 36+offsetY, COLOR_FOREGROUND);
  display.drawLine(15+offsetX, 43+offsetY, 70+offsetX, 43+offsetY, COLOR_FOREGROUND);
  //display.drawLine(20+offsetX, 50+offsetY, 67+offsetX, 50+offsetY, COLOR_FOREGROUND);
  display.drawLine(23+offsetX, 50+offsetY, 60+offsetX, 50+offsetY, COLOR_FOREGROUND);
  display.drawLine(24+offsetX, 57+offsetY, 80+offsetX, 57+offsetY, COLOR_FOREGROUND);
  display.drawLine(37+offsetX, 64+offsetY, 67+offsetX, 64+offsetY, COLOR_FOREGROUND);
}
void EPaperWeatherDrawer::DrawFogg_Small(int offsetX, int offsetY) 
{
  display.drawLine(15+offsetX, 18+offsetY, 33+offsetX, 18+offsetY, COLOR_FOREGROUND);
  display.drawLine(7+offsetX, 22+offsetY, 35+offsetX, 22+offsetY, COLOR_FOREGROUND);
  display.drawLine(11+offsetX, 26+offsetY, 30+offsetX, 26+offsetY, COLOR_FOREGROUND);
  display.drawLine(12+offsetX, 30+offsetY, 40+offsetX, 30+offsetY, COLOR_FOREGROUND);
  display.drawLine(19+offsetX, 34+offsetY, 33+offsetX, 34+offsetY, COLOR_FOREGROUND);
}

void EPaperWeatherDrawer::DrawMediumSun(int offsetX, int offsetY) 
{
  int middleX = 30 + offsetX;  
  int middleY = 30 + offsetY;

  int length = 60;

  display.drawLine(middleX-length/2, middleY, middleX+length/2, middleY, COLOR_FOREGROUND);//horz
  display.drawLine(middleX, middleY-length/2, middleX, middleY+length/2, COLOR_FOREGROUND);//vert

  for(int angle = 30; angle < 180; angle+=30) {
    if (angle == 90)
      continue;
      
    DrawPolarRay(middleX, middleY, length, angle);
  }

  int radiusSun = 17;
  
  display.fillCircle(middleX, middleY, radiusSun, COLOR_RED);
  display.drawCircle(middleX, middleY, radiusSun, COLOR_FOREGROUND);
}
void EPaperWeatherDrawer::DrawMediumSun_Small(int offsetX, int offsetY) 
{
  int middleX = 15 + offsetX;  
  int middleY = 15 + offsetY;

  int length = 30;

  display.drawLine(middleX-length/2, middleY, middleX+length/2, middleY, COLOR_FOREGROUND);//horz
  display.drawLine(middleX, middleY-length/2, middleX, middleY+length/2, COLOR_FOREGROUND);//vert

  for(int angle = 30; angle < 180; angle+=30) {
    if (angle == 90)
      continue;
      
    DrawPolarRay(middleX, middleY, length, angle);
  }

  int radiusSun = 8;
  
  display.fillCircle(middleX, middleY, radiusSun, COLOR_RED);
  display.drawCircle(middleX, middleY, radiusSun, COLOR_FOREGROUND);
}  

void EPaperWeatherDrawer::DrawMediumMoon(int offsetX, int offsetY) 
{
  offsetX+=3;
  
  const int radiusMoon = 20;
  
  display.fillCircle(45+offsetX, 45+offsetY, radiusMoon, COLOR_RED);
  display.drawCircle(45+offsetX, 45+offsetY, radiusMoon, COLOR_FOREGROUND);
  
  const int offset = 10;
  display.fillCircle(45+offsetX-offset, 45+offsetY-offset, radiusMoon, COLOR_BACKGROUND);
  display.drawCircle(45+offsetX-offset, 45+offsetY-offset, radiusMoon, COLOR_FOREGROUND);
  
  // display.drawCircle(45+offsetX-offset-radiusMoon,        45+offsetY-offset-radiusMoon, 1, COLOR_RED);
  // display.drawCircle(45+offsetX-offset+radiusMoon+offset, 45+offsetY-offset-radiusMoon, 1, COLOR_RED);
  // display.drawCircle(45+offsetX-offset-radiusMoon,        45+offsetY-offset+radiusMoon+offset, 1, COLOR_RED);

  display.fillTriangle(45+offsetX-offset-radiusMoon,        45+offsetY-offset-radiusMoon, 
                       45+offsetX-offset+radiusMoon+offset, 45+offsetY-offset-radiusMoon, 
                       45+offsetX-offset-radiusMoon,        45+offsetY-offset+radiusMoon+offset, COLOR_BACKGROUND);
}
void EPaperWeatherDrawer::DrawMediumMoon_Small(int offsetX, int offsetY) 
{
  offsetX+=32;
  offsetY+=29;
  
  const int radiusMoon = 10;
  
  display.fillCircle(offsetX, offsetY, radiusMoon, COLOR_RED);
  display.drawCircle(offsetX, offsetY, radiusMoon, COLOR_FOREGROUND);
  
  const int offset = 6;
  display.fillCircle(offsetX-offset, offsetY-offset, radiusMoon, COLOR_BACKGROUND);
  display.drawCircle(offsetX-offset, offsetY-offset, radiusMoon, COLOR_FOREGROUND);
  
  // display.drawCircle(offsetX-offset-radiusMoon,        offsetY-offset-radiusMoon, 1, COLOR_RED);
  // display.drawCircle(offsetX-offset+radiusMoon+offset, offsetY-offset-radiusMoon, 1, COLOR_RED);
  // display.drawCircle(offsetX-offset-radiusMoon,        offsetY-offset+radiusMoon+offset, 1, COLOR_RED);

  display.fillTriangle(offsetX-offset-radiusMoon,        offsetY-offset-radiusMoon, 
                       offsetX-offset+radiusMoon+offset, offsetY-offset-radiusMoon, 
                       offsetX-offset-radiusMoon,        offsetY-offset+radiusMoon+offset, COLOR_BACKGROUND);
}

void EPaperWeatherDrawer::DrawSmallSun(int offsetX, int offsetY) 
{
  int middleX = 26 + offsetX;  
  int middleY = 26 + offsetY;

  int length = 53;

  display.drawLine(middleX-length/2, middleY, middleX+length/2, middleY, COLOR_FOREGROUND);//horz
  display.drawLine(middleX, middleY-length/2, middleX, middleY+length/2, COLOR_FOREGROUND);//vert

  for(int angle = 30; angle < 180; angle+=30) {
    if (angle == 90)
      continue;
      
    DrawPolarRay(middleX, middleY, length, angle);
  }

  int radiusSun = 15;
  
  display.fillCircle(middleX, middleY, radiusSun, COLOR_RED);
  display.drawCircle(middleX, middleY, radiusSun, COLOR_FOREGROUND);
}
void EPaperWeatherDrawer::DrawSmallSun_Small(int offsetX, int offsetY) 
{
  int middleX = 14 + offsetX;  
  int middleY = 14 + offsetY;

  int length = 26;

  display.drawLine(middleX-length/2, middleY, middleX+length/2, middleY, COLOR_FOREGROUND);//horz
  display.drawLine(middleX, middleY-length/2, middleX, middleY+length/2, COLOR_FOREGROUND);//vert

  for(int angle = 30; angle < 180; angle+=30) {
    if (angle == 90)
      continue;
      
    DrawPolarRay(middleX, middleY, length, angle);
  }

  int radiusSun = 8;
  
  display.fillCircle(middleX, middleY, radiusSun, COLOR_RED);
  display.drawCircle(middleX, middleY, radiusSun, COLOR_FOREGROUND);
}

void EPaperWeatherDrawer::DrawSmallMoon(int offsetX, int offsetY) 
{
  offsetX+=3;
  
  const int radiusMoon = 18;
  
  display.fillCircle(45+offsetX, 45+offsetY, radiusMoon, COLOR_RED);
  display.drawCircle(45+offsetX, 45+offsetY, radiusMoon, COLOR_FOREGROUND);
  
  const int offset = 9;
  display.fillCircle(45+offsetX-offset, 45+offsetY-offset, radiusMoon, COLOR_BACKGROUND);
  display.drawCircle(45+offsetX-offset, 45+offsetY-offset, radiusMoon, COLOR_FOREGROUND);

  display.fillTriangle(45+offsetX-offset-radiusMoon,        45+offsetY-offset-radiusMoon, 
                       45+offsetX-offset+radiusMoon+offset, 45+offsetY-offset-radiusMoon, 
                       45+offsetX-offset-radiusMoon,        45+offsetY-offset+radiusMoon+offset, COLOR_BACKGROUND);

}
void EPaperWeatherDrawer::DrawSmallMoon_Small(int offsetX, int offsetY) 
{
  offsetX+=48;
  offsetY+=45;
  
  const int radiusMoon = 9;
  
  display.fillCircle(offsetX, offsetY, radiusMoon, COLOR_RED);
  display.drawCircle(offsetX, offsetY, radiusMoon, COLOR_FOREGROUND);
  
  const int offset = 4;
  display.fillCircle(offsetX-offset, offsetY-offset, radiusMoon, COLOR_BACKGROUND);
  display.drawCircle(offsetX-offset, offsetY-offset, radiusMoon, COLOR_FOREGROUND);

  display.fillTriangle(offsetX-offset-radiusMoon,        offsetY-offset-radiusMoon, 
                       offsetX-offset+radiusMoon+offset, offsetY-offset-radiusMoon, 
                       offsetX-offset-radiusMoon,        offsetY-offset+radiusMoon+offset, COLOR_BACKGROUND);
}

void EPaperWeatherDrawer::DrawWhiteCloud(int offsetX, int offsetY) 
{
  display.drawCircle(10+offsetX, 25+offsetY, 8, COLOR_FOREGROUND);
  display.drawCircle(25+offsetX, 19+offsetY, 14, COLOR_FOREGROUND);
  display.drawCircle(43+offsetX, 17+offsetY, 16, COLOR_FOREGROUND);
  display.drawCircle(60+offsetX, 23+offsetY, 10, COLOR_FOREGROUND);

  display.fillCircle(10+offsetX, 25+offsetY, 7, COLOR_BACKGROUND);
  display.fillCircle(25+offsetX, 19+offsetY, 13, COLOR_BACKGROUND);
  display.fillCircle(43+offsetX, 17+offsetY, 15, COLOR_BACKGROUND);
  display.fillCircle(60+offsetX, 23+offsetY, 9, COLOR_BACKGROUND);
  
  display.fillRect(10+offsetX, 28+offsetY, 50, 5, COLOR_BACKGROUND);
  display.drawLine(10+offsetX, 33+offsetY, 60+offsetX, 33+offsetY, COLOR_FOREGROUND);
}
void EPaperWeatherDrawer::DrawWhiteCloud_Small(int offsetX, int offsetY) 
{

  display.drawCircle(5+offsetX, 12+offsetY, 4, COLOR_FOREGROUND);
  display.drawCircle(12+offsetX, 10+offsetY, 7, COLOR_FOREGROUND);
  display.drawCircle(18+offsetX,  9+offsetY, 8, COLOR_FOREGROUND);
  display.drawCircle(26+offsetX, 12+offsetY, 4, COLOR_FOREGROUND);

  display.fillCircle(5+offsetX, 12+offsetY, 3, COLOR_BACKGROUND);
  display.fillCircle(12+offsetX, 10+offsetY, 6, COLOR_BACKGROUND);
  display.fillCircle(18+offsetX,  9+offsetY, 7, COLOR_BACKGROUND);
  display.fillCircle(26+offsetX, 12+offsetY, 3, COLOR_BACKGROUND);
  
  display.fillRect(6+offsetX, 14+offsetY, 21, 4, COLOR_BACKGROUND);
  display.drawLine(5+offsetX, 16+offsetY, 26+offsetX, 16+offsetY, COLOR_FOREGROUND);
}

void EPaperWeatherDrawer::DrawBlackCloud(int offsetX, int offsetY) 
{
  display.fillCircle(10+offsetX, 25+offsetY, 8, COLOR_FOREGROUND);
  display.fillCircle(25+offsetX, 19+offsetY, 14, COLOR_FOREGROUND);
  display.fillCircle(43+offsetX, 17+offsetY, 16, COLOR_FOREGROUND);
  display.fillCircle(60+offsetX, 23+offsetY, 10, COLOR_FOREGROUND);

  display.fillRect(10+offsetX, 28+offsetY, 50, 6, COLOR_FOREGROUND);
}
void EPaperWeatherDrawer::DrawBlackCloud_Small(int offsetX, int offsetY) {

  display.fillCircle(5+offsetX, 12+offsetY, 4, COLOR_FOREGROUND);
  display.fillCircle(12+offsetX, 10+offsetY, 7, COLOR_FOREGROUND);
  display.fillCircle(18+offsetX,  9+offsetY, 8, COLOR_FOREGROUND);
  display.fillCircle(26+offsetX, 12+offsetY, 4, COLOR_FOREGROUND);

  display.fillRect(6+offsetX, 14+offsetY, 21, 4, COLOR_FOREGROUND);
}

void EPaperWeatherDrawer::DrawPolarRay(int middleX, int middleY, int length, int angle) 
{
  int dX = length/2 * cos(radians(angle));
  int dY = length/2 * sin(radians(angle));  

  display.drawLine(middleX + dX, middleY + dY, middleX - dX, middleY - dY, COLOR_FOREGROUND);
}