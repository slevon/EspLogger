

#include "rrapsettings.h"


RRApSettings::RRApSettings(){
  }

RRApSettings::~RRApSettings(){
  
  }




void RRApSettings::saveSettings(){
    EEPROM.begin(512);
    EEPROM.put(RRAPSETTINGS_EEPROMSTART,settings);
    delay(200);
    EEPROM.commit();
    EEPROM.end();
  }

void RRApSettings::restoreSettings(){
    
    EEPROM.begin(512);
    EEPROM.get(RRAPSETTINGS_EEPROMSTART,settings);
    EEPROM.end();
    Serial.println(F("-----"));
    Serial.println(F("Settings are:"));
    Serial.print(F("ssid:"));
    Serial.println(settings.ssid);
    Serial.print(F("pass:"));
    Serial.println(settings.pass);
    Serial.println(settings.enableHttpSend?"true (1)":"false (0)");
    
  }

