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
#define RRSETTINGS_BUFFER_SIZE 250
class RRSettings{
  public:
    RRSettings();
    ~RRSettings();
    
    char** keys;
    char** values;
    unsigned short mapSize;  //size of LUT

    //bool save();
    bool saveServerArgs(ESP8266WebServer& server);
    bool load(String filename);
    String wifiList();
    //static String urldecode(String url);
    String toJson();
    //String operator [](String key){ if(configLoaded){return (*data)[key].asString();} return "";}
    //String & operator [](String key) const { if(configLoaded){return(*data)[key].as<char*>();} data = &jsonBuffer.parseObject(String("{}")); return (*data)[key].as<char*>();}
    String get(String key);
    //void set(char* key,char* value);
    bool getBool(char* key,bool defaultValue=false);
    long getLong(char* key, long defaultValue=-1);
    
   
  private:
    short indexOf(char* key);
    String value(char* key);
    void valueRef(char* key, char*ref);
    static unsigned char h2int(char c);
    void print();
    String currentFilename;
    bool configLoaded; //is false if load has not been called or was unsucesfull
  };

#endif
