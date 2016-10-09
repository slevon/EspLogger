#ifndef RRSETTINGS
#define RRSETTINGS

#include "main.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "FS.h"

#define RRSETTINGS_DOUBLE_LONG_STRING 128
#define RRSETTINGS_LONG_STRING 64
#define RRSETTINGS_SHORT_STRING 32
#define RRSETTINGS_BUFFER_SIZE 512
class RRSettings{
  public:
    RRSettings();
    ~RRSettings();
    JsonVariant data;
    StaticJsonBuffer<RRSETTINGS_BUFFER_SIZE> jsonBuffer;
    std::unique_ptr<char[]> buf;

    bool save();
    bool saveServerArgs(ESP8266WebServer& server);
    bool load(String filename);
    String toJson(){String d; data.prettyPrintTo(d); return d;} 
    String wifiList();
    static String urldecode(String url);
    //String operator [](String key){ if(configLoaded){return (data.asObject())[key].asString();} return "";}
    //String & operator [](String key) const { if(configLoaded){return(data.asObject())[key].as<char*>();} data = &jsonBuffer.parseObject(String("{}")); return (data.asObject())[key].as<char*>();}
    String get(String key){if((data.asObject()).containsKey(key)){return (data.asObject())[key].asString();} Serial.println("Error: Unknown get() "+key); data.asObject().printTo(Serial); return "";}
    void set(String key,JsonVariant value){if(!configLoaded){ data = &jsonBuffer.parseObject(String("{}")); }  (data.asObject())[key]=value;}
    bool getBool(String key,bool defaultValue=false){if(configLoaded) {return (data.asObject())[key].as<bool>();} return defaultValue;}
    long getLong(String key, long defaultValue=-1){if(configLoaded){return (data.asObject())[key].as<long>();} return defaultValue;}
    
  private:
    static unsigned char h2int(char c);
    String currentFilename;
    bool configLoaded; //is false if load has not been called or was unsucesfull
  };

#endif
