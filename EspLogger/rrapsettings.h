#ifndef RRAPSETTINGS
#define RRAPSETTINGS

#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define RRAPSETTINGS_EEPROMSTART 0 // from wher on do we read/write

class RRApSettings{
  public:
    struct{
      char ssid[64];//defined max length
      char pass[33];//definde max length
      byte enableHttpSend;
      char httpSendUrl[100];
      unsigned int  httpSendInterval; //in minutes:
      //ntp:
      byte ntpEnable;
      unsigned ntpInterval; //!Achtung: in hours
      
      }settings;
  
    RRApSettings();
    ~RRApSettings();

    void save();
    void restore();
    String wifiList();
    static String urldecode(String url);

  private:
  static unsigned char h2int(char c);

  
  };

#endif
