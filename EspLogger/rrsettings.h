#ifndef RRSETTINGS
#define RRSETTINGS

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "FS.h"

#define RRSETTINGS_DOUBLE_LONG_STRING 128
#define RRSETTINGS_LONG_STRING 64
#define RRSETTINGS_SHORT_STRING 32
class RRSettings{
  public:
    RRSettings();
    ~RRSettings();
    JsonObject& data;

    bool save();
    bool saveServerArgs(ESP8266WebServer& server);
    bool load(String name);
    String toJson(){String d; data.prettyPrintTo(d); return d;} 
    String wifiList();
    static String urldecode(String url);

  private:
    static unsigned char h2int(char c);
    String currentFilename;
  };

#endif
