
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

String RRApSettings::wifiList(){
    String Wifis = "";
    String selected="";
    Serial.println("scan start");
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0){
      Serial.println("no networks found");
      Wifis="<option disabled>No Networks</option>";
    }else{
      Serial.print(n);
      Serial.println(" networks found");
      String signal = "";
      Wifis += "<select>\n";
      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        //Signal good?
        if (WiFi.RSSI(i) <= -80){
          signal = "&#128533";//poor
        }else if (i <= -60){
          signal = "&#128528"; //medium
        }else{
          signal = "&#128522"; //good
        }
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        if(settings.ssid == WiFi.SSID(i)){
          selected = "selected";
        }else{
          selected = "";
        }
        Wifis += String("<option value='"+String(WiFi.SSID(i))+"' "+selected+">" + String(WiFi.SSID(i)) + " - Signalstärke: " + String(signal) + "</option>\n");
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      }
      Wifis += "</select>\n";
      Serial.println();
      Serial.println(Wifis);
    }

    return Wifis;
  }

  
