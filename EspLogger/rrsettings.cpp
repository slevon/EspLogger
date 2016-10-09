
#include "rrsettings.h"

RRSettings::RRSettings() {
  configLoaded=false;
}

RRSettings::~RRSettings() {

}

bool RRSettings::save() {
  File configFile = SPIFFS.open(currentFilename, "w");
  if (!configFile) {
    DEBUGPRINT.println("Failed to open config file for writing");
     configFile.close();
    return false;
  }
 data.asObject().printTo(DEBUGPRINT);
  data.asObject().printTo(configFile);
  configFile.close();
  DEBUGPRINT.println("Config saved");
  return true;
}

bool RRSettings::saveServerArgs(ESP8266WebServer& server){

  if(!configLoaded){
    //create new object
    data=&jsonBuffer.createObject();
    }
   for ( uint8_t i = 0; i < server.args(); i++ ) {
      DEBUGPRINT.println(server.argName ( i ) + ": " + server.arg ( i ));
      DEBUGPRINT.println();
        (data.asObject())[server.argName(i)]=RRSettings::urldecode(server.arg(i).c_str());
      
      }
    return save();
}

bool  RRSettings::load(String filename) {
  configLoaded=false;
  currentFilename = String("/config/")+filename+".json";
  File configFile = SPIFFS.open(currentFilename, "r");
  
  if (!configFile) {
    DEBUGPRINT.println("Failed to open config file: "+filename);
    configFile.close();
    return false;
  }
  
  size_t size = configFile.size();
  if (size > RRSETTINGS_BUFFER_SIZE) {
    DEBUGPRINT.println("Config file size is too large");
    configFile.close();
    return false;
  }
  
  // Allocate a buffer to store contents of the file.
  buf.reset((new char[size]));
  
  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);
  data = jsonBuffer.parseObject(buf.get());
  
  if (!data.success()) {
    DEBUGPRINT.println("Failed to parse config file");
    return false;
  }
  
  DEBUGPRINT.println("Loaded Config: "+filename);
  data.asObject().printTo(DEBUGPRINT);
  DEBUGPRINT.println();
  //for(JsonObject::iterator it=data->begin(); it!=data->end();++it){
  //  DEBUGPRINT.print(it->key);
  //  DEBUGPRINT.print(": ");
  //  DEBUGPRINT.print(it->value.asString()+"\n");
  //  }
  configFile.close();
  configLoaded=true;
  return true;
  
}

String RRSettings::wifiList() {
  String Wifis = "";
  String selected = "";
  DEBUGPRINT.println(F("\nscan start"));
  int n = WiFi.scanNetworks();
  DEBUGPRINT.println(F("scan done"));
  if (n == 0) {
    DEBUGPRINT.println(F("no networks found"));
    Wifis = F("<option disabled>No Networks</option>");
  } else {
    DEBUGPRINT.print(n);
    DEBUGPRINT.println(" networks found");
    Wifis = F("<option value=''>Deaktiveren</option>");
    String signal = "";
     load("wifi");
     String currentWifi=get("ssid");
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
      DEBUGPRINT.print(i + 1);
      DEBUGPRINT.print(": ");
      DEBUGPRINT.print(WiFi.SSID(i));
     
      if ( currentWifi == WiFi.SSID(i)) {
        selected = "selected";
      } else {
        selected = "";
      }
      Wifis += String("<option value='" + String(WiFi.SSID(i)) + "' " + selected + ">" + String(WiFi.SSID(i)) + " - Signalstärke: " + String(signal) + "</option>\n");
      DEBUGPRINT.print(" (");
      DEBUGPRINT.print(WiFi.RSSI(i));
      DEBUGPRINT.print(")");
      DEBUGPRINT.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
    }
    DEBUGPRINT.println();
    DEBUGPRINT.println(Wifis);
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

