
#include "rrsettings.h"

RRSettings::RRSettings() {
  configLoaded=false;
}

RRSettings::~RRSettings() {

}

bool RRSettings::save() {
  File configFile = SPIFFS.open(currentFilename, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
     configFile.close();
    return false;
  }

  data->printTo(configFile);
  configFile.close();
  return true;
}

bool RRSettings::saveServerArgs(ESP8266WebServer& server){

  if(!configLoaded){
    //create new object
    data=&jsonBuffer.parseObject("{}");
    }
   for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.println(server.argName ( i ) + ": " + server.arg ( i ));
      Serial.println();
        (*data)[server.argName(i)]=RRSettings::urldecode(server.arg(i).c_str());
      
      }
    return save();
}

bool  RRSettings::load(String name) {
  configLoaded=false;
  currentFilename = String("/config/")+name+".json";
  File configFile = SPIFFS.open(currentFilename, "r");
  
  if (!configFile) {
    Serial.println("Failed to open config file");
    configFile.close();
    return false;
  }
  
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    configFile.close();
    return false;
  }
  
  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);
  
  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);
  
  data = &jsonBuffer.parseObject(buf.get());
  
  if (!data->success()) {
    Serial.println("Failed to parse config file");
    return false;
  }
  
  Serial.println("Loaded Config: "+name);
  for(JsonObject::iterator it=data->begin(); it!=data->end();++it){
    Serial.print(it->key);
    Serial.print(": ");
    Serial.print(it->value.asString());
    }
  configFile.close();
  configLoaded=true;
  return true;
  
}

String RRSettings::wifiList() {
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
    Wifis = F("<option value=''>Deaktiveren</option>");
    String signal = "";
     load("wifi");
     String currentWifi=(*data)["ssid"];
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
     
      if ( currentWifi == WiFi.SSID(i)) {
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


String RRSettings::urldecode(String str) {

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

unsigned char  RRSettings::h2int(char c) {
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

