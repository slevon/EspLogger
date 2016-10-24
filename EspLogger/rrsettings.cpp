
#include "rrsettings.h"

RRSettings::RRSettings() {
  configLoaded=false;
}

RRSettings::~RRSettings() {

}
 /*
bool RRSettings::save() {

  * 
  u/s seit wechesl auf eigene MAP File configFile = SPIFFS.open(currentFilename, "w");
  if (!configFile) {
    DEBUGPRINT.println("Failed to open config file for writing");
     configFile.close();
    return false;
  }
  configFile.close();
  DEBUGPRINT.println("Config saved");
  return true;
  
  return false;
}
*/

bool RRSettings::saveServerArgs(ESP8266WebServer& server){
  Serial.println("Saving: ");
    currentFilename = String("/config/")+server.arg("rrsection")+".json";
    StaticJsonBuffer<RRSETTINGS_BUFFER_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
   for ( uint8_t i = 0; i < server.args(); i++ ) {
      DEBUGPRINT.println(server.argName ( i ) + ": " + server.arg ( i ));
      DEBUGPRINT.println();
      root[server.argName ( i )] = server.arg (i);
      }
      File configFile = SPIFFS.open(currentFilename, "w");
      root.printTo(Serial);
      root.printTo(configFile);
      configFile.close();
      return true; //TODO
}

bool  RRSettings::load(String filename) {
  if(configLoaded){
     //DELETE!
    for(int i=0; i<mapSize; ++i){
        delete[] keys[i];
        delete[] values[i];
    }
    delete[] keys;
    delete[] values;
    keys = NULL;
    values = NULL;
    }
  configLoaded=false;
  mapSize=0;
  currentFilename = String("/config/")+filename+".json";
  File configFile = SPIFFS.open(currentFilename, "r");
  
  if (!configFile) {
    DEBUGPRINT.println("Failed to open config file: "+filename);
    configFile.close();
    return false;
  }else{
    Serial.println("Config "+currentFilename+" opened");
    }
  
  size_t size = configFile.size();
  if (size > RRSETTINGS_BUFFER_SIZE) {
    DEBUGPRINT.println("Config file size is too large");
    configFile.close();
    return false;
  }
 
  // Allocate a buffer to store contents of the file.
  char* buf=new char[size];
  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf, size);
  StaticJsonBuffer<RRSETTINGS_BUFFER_SIZE> jsonBuffer;
  JsonObject& data=jsonBuffer.parseObject(buf);
 // delete[] buf;
  
  if (!data.success()) {
    DEBUGPRINT.println("Failed to parse config file");
    data.printTo(DEBUGPRINT);
    return false;
  }
  
  DEBUGPRINT.println("\nLoaded Config: "+filename);
  data.printTo(DEBUGPRINT);
  DEBUGPRINT.println();
  int LEN=data.size();
  keys = new char*[LEN];  //a dynamic array of dynamic strings
  values = new char*[LEN];  //a dynamic array of dynamic strings
  int i=0;
  for(JsonObject::iterator it=data.begin(); it!=data.end();++it){
    DEBUGPRINT.print(it->key);
    DEBUGPRINT.print(": ");
    DEBUGPRINT.println(it->value.asString());
   
  //for(int i=0; i<LEN; ++i) {
      const char* val=it->value.as<const char*>();
      keys[i] = new char[strlen(it->key)+1];
      values[i] = new char[strlen(val)+1];
      strcpy(keys[i], it->key);
      strcpy(values[i],val);
  //}
   // Serial.print("KOPIE:::::: ");
   // Serial.print(keys[i]);
   // Serial.print("->");
   // Serial.println(values[i]);
      i++;
    }
    //store the mapsize
  mapSize=i;
  configFile.close();
  configLoaded=true;
  delete[] buf;
  print();
  return true;
  
}



String RRSettings::wifiList() {
  String Wifis = "";
  String selected = "";
  int n = WiFi.scanNetworks();
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
     bool currentWifiFound=false;
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
        currentWifiFound =true;
      } else {
        selected = "";
      }
      Wifis += String("<option value='" + String(WiFi.SSID(i)) + "' " + selected + ">" + String(WiFi.SSID(i)) + " - Signalstärke: " + String(signal) + "</option>\n");
      DEBUGPRINT.print(" (");
      DEBUGPRINT.print(WiFi.RSSI(i));
      DEBUGPRINT.print(")");
      DEBUGPRINT.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
    }
    if(!currentWifiFound){
      //appedn current Wifi, even if it is not in range, so to hold current config
       Wifis += String("<option value='" + currentWifi + "' selected >" + currentWifi + " - (keep)</option>\n");
    }
    DEBUGPRINT.println();
    DEBUGPRINT.println(Wifis);
  }

  return Wifis;
}

/*
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
  }

  return encodedString;
}
*/
String RRSettings::toJson(){
  String ret="{";
  for(int i=0;i<mapSize;i++){
    ret+=String("\"")+keys[i]+"\":\""+values[i]+"\","; 
   }
   ret.setCharAt(ret.length()-1,'}');
   return ret;
   
  }
    
String RRSettings::get(String key){
  if(configLoaded){
    return value((char*)key.c_str());
   } 
   Serial.println("Error: Unknown get() "+key); 
   
   return "";
  }

  /*
void RRSettings::set(char* key,char* value){
    short idx=indexOf(key);
    if(idx>=0){
      //We have the value;
      char* newValue=new char[strlen(value)+1];
      strcpy(newValue,value);
      delete[] values[idx];
      values[idx]=newValue;
    }else{
      Serial.println("New KEY");
        //we don't have it yet
        mapSize++;
        char** newKeys=new char* [mapSize];
        char** newValues=new char* [mapSize];
        char* newKey=new char [sizeof(key)];
        char* newValue=new char [sizeof(value)];
        strcpy(newKey,key);
        strcpy(newValue,value);
        newKeys[0]=newKey;
        newValues[0]=newValue;
        Serial.println("Created new MAtrix");
        
        for(int i=1;i<mapSize;i++){
            
            newKey = new char [sizeof(keys[i])];
            newValue = new char [sizeof(values[i])];
            strcpy(newKey,keys[i-1]);
            strcpy(newValue,values[i-1]);
            delete[] keys[i];
            delete[] values[i];
            keys[i]=NULL;
            values[i]=NULL;
            newKeys[i]=newKey;
            newValues[i]=newValue;
            Serial.print("Copy ");
            Serial.print(newKey);
            Serial.println(i);
        }
        Serial.println("Delete");
        delete[] keys;
        delete[] values;
        keys=NULL;
        values=NULL;
        keys=newKeys;
        values=newValues;
        Serial.println("Delete2");
       
        
        
    }
  }
  */
bool RRSettings::getBool(char* key,bool defaultValue){
  if(configLoaded){
    if(value(key).equalsIgnoreCase("true")){
      return true;
      }
   } 
   return defaultValue;
  }
long RRSettings::getLong(char* key, long defaultValue){
    if(configLoaded){
        return value(key).toInt();
    } 
    return defaultValue;
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


short RRSettings::indexOf(char* key){
  for(int i = 0; i< mapSize; i++){
      if(strcmp(key,keys[i]) == 0){
        return i; 
       }
    }
    Serial.println("No Index found");
    return -1;
}

String RRSettings::value(char* key){
  short idx=indexOf(key);
  if(idx>=0){
    return String(values[idx]);
  }
  Serial.println("No Value");
  return "";
}

void RRSettings::print(){
  Serial.println("##Config## "+currentFilename);
  for(int i = 0; i< mapSize; i++){
      Serial.print(i);
      Serial.print(": ");
      Serial.print(keys[i]);
      Serial.print(" - ");
      Serial.println(values[i]);
       }
  }
