#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//#include <WiFi.h>
//#include <HTTPClient.h>

class JsonStreamingParser;

class WifiParser
{
public:
  static bool setup();

  static bool parse(JsonStreamingParser* pParser);

private:
  bool static httpGETRequest(const char* szRequest, JsonStreamingParser* pParser);
};