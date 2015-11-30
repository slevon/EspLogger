#ifndef RRAPSETTINGS
#define RRAPSETTINGS

#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define RRAPSETTINGS_EEPROMSTART 0 // from wher on do we read/write



class RRApSettings{
  public:
    struct{
      char ssid[24];
      char pass[24];
      byte enableHttpSend;
      char httpSendUrl[100];
      unsigned int  httpSendInterval; //in seconds
      
      }settings;
  
    RRApSettings();
    ~RRApSettings();

    void saveSettings();
    void restoreSettings();
  
  };

#endif
