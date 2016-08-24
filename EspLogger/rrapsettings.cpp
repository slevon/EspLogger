
#include "rrapsettings.h"

RRApSettings::RRApSettings() {
}

RRApSettings::~RRApSettings() {

}




void RRApSettings::save() {
  EEPROM.begin(1024);
  EEPROM.put(RRAPSETTINGS_EEPROMSTART, settings);
  delay(200);
  EEPROM.commit();
  EEPROM.end();
}

void RRApSettings::restore() {
  EEPROM.begin(1024);
  EEPROM.get(RRAPSETTINGS_EEPROMSTART, settings);
  EEPROM.end();
  //chek if we have valid settings here... Else clear them:
  if ((int)settings.ssid[0] > 123 || (int)settings.pass[0] > 127) {
    settings.ssid[0] = 0;
    settings.pass[0] = 0;
    settings.httpSendUrl[0] = 0;
    save();
    Serial.println("USING Settings-Fallback");
  }

  Serial.println(F("-----"));
  Serial.println(F("Settings are:"));
  Serial.print(F("ssid:"));
  Serial.println(settings.ssid);
  Serial.print(F("pass:"));
  Serial.println(settings.pass);
  Serial.print(F("HTTP send:"));
  Serial.println(settings.enableHttpSend ? F("true (1)") : F("false (0)"));
  Serial.print(F("HTTP URL:"));
  Serial.println(settings.httpSendUrl);
  Serial.print(F("HTTP interval/min:"));
  Serial.println(settings.httpSendInterval); //in minutes:
  Serial.print(F("NTP/INTERVAL:"));
  Serial.print(settings.ntpEnable ? F("true (1) ") : F("false (0) "));
  Serial.println(settings.ntpInterval);
  Serial.print("Telegramm: ");
  Serial.println(settings.BotEnable? F("true (1) ") : F("false (0) "));
  Serial.println(settings.BotToken);
  Serial.println(settings.BotName);
  Serial.println(settings.BotUsername);
  Serial.println(settings.BotChatId);
  Serial.print("Email: ");
  Serial.println(settings.mailEnable? F("true (1) ") : F("false (0) "));
  Serial.println(settings.mailReceiver);
  Serial.print("Device: ");
  Serial.println(settings.deviceName);
  Serial.println(settings.deviceId);
  Serial.println(settings.deviceFloor);
  Serial.println(settings.deviceRoom);
  
}

String RRApSettings::wifiList() {
  String Wifis = "";
  String selected = "";
  Serial.println(F("scan start"));
  int n = WiFi.scanNetworks();
  Serial.println(F("scan done"));
  if (n == 0) {
    Serial.println(F("no networks found"));
    Wifis = F("<option disabled>No Networks</option>");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    String signal = "";
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      //Signal good?
      if (WiFi.RSSI(i) <= -80) {
        signal = "&#128533";//poor
      } else if (i <= -60) {
        signal = "&#128528"; //medium
      } else {
        signal = "&#128522"; //good
      }
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      if (String(settings.ssid) == WiFi.SSID(i)) {
        selected = "selected";
      } else {
        selected = "";
      }
      Wifis += String("<option value='" + String(WiFi.SSID(i)) + "' " + selected + ">" + String(WiFi.SSID(i)) + " - Signalstärke: " + String(signal) + "</option>\n");
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
    }
    Serial.println();
    Serial.println(Wifis);
  }

  return Wifis;
}


String RRApSettings::urldecode(String str) {

  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {

      encodedString += c;
    }

    yield();
  }

  return encodedString;
}

unsigned char  RRApSettings::h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

