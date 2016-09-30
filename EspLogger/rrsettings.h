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
    JsonObject* data;
    StaticJsonBuffer<500> jsonBuffer;

    bool save();
    bool saveServerArgs(ESP8266WebServer& server);
    bool load(String name);
    String toJson(){String d; data->prettyPrintTo(d); return d;} 
    String wifiList();
    static String urldecode(String url);
    //String operator [](String key){ if(configLoaded){return (*data)[key].asString();} return "";}
    //String & operator [](String key) const { if(configLoaded){return(*data)[key].as<char*>();} data = &jsonBuffer.parseObject(String("{}")); return (*data)[key].as<char*>();}
    String get(String key){if(configLoaded){return (*data)[key].asString();} return "";}
    void set(String key,JsonVariant value){if(!configLoaded){ data = &jsonBuffer.parseObject(String("{}")); }  (*data)[key]=value;}
    bool getBool(String key){if(configLoaded) {return (*data)[key].as<bool>();} return false;}
    long getLong(String key){if(configLoaded){return (*data)[key].as<long>();} return -1;}
    
  private:
    static unsigned char h2int(char c);
    String currentFilename;
    bool configLoaded; //is false if load has not been called or was unsucesfull
  };

#endif
