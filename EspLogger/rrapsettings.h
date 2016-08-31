#ifndef RRAPSETTINGS
#define RRAPSETTINGS

#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define RRAPSETTINGS_EEPROMSTART 0 // from wher on do we read/write
#define RRAPSETTINGS_DOUBLE_LONG_STRING 128
#define RRAPSETTINGS_LONG_STRING 64
#define RRAPSETTINGS_SHORT_STRING 32
class RRApSettings{
  public:
    struct{
      char ssid[RRAPSETTINGS_LONG_STRING];//defined max length
      char pass[33];//definde max length
      byte enableHttpSend;
      char httpSendUrl[RRAPSETTINGS_DOUBLE_LONG_STRING];
      unsigned int  httpSendInterval; //in minutes:
      //ntp:
      byte ntpEnable;
      unsigned ntpInterval; //!Achtung: in hours
      //Telegram
      byte BotEnable;  //
      char BotToken[RRAPSETTINGS_LONG_STRING];  //token of TestBOT
      char BotName[RRAPSETTINGS_SHORT_STRING]; 
      char BotUsername[RRAPSETTINGS_SHORT_STRING];
      char BotChatId[RRAPSETTINGS_SHORT_STRING];
      //Mail
      byte mailEnable;
      char mailReceiver[RRAPSETTINGS_DOUBLE_LONG_STRING];
      //device
      byte deviceFloor; //Etage
      unsigned int deviceId;
      char deviceRoom[RRAPSETTINGS_SHORT_STRING];
      char deviceName[RRAPSETTINGS_SHORT_STRING];
      //Relay
      byte relayEnable;
      char relayChangeUrl[RRAPSETTINGS_LONG_STRING];
      
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
