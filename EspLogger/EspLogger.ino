/*
 *  This sketch creates a IOT Thing
 */

#include "main.h"


#include <TimeLib.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "FS.h"
//Telegram
#include <WiFiClientSecure.h>
#include <TelegramBot.h>

//Blynk
#include <BlynkSimpleEsp8266.h>
char blynkAuth[] = "e9f08daf80e3486a81a3c2664f271999"; //dummy allocation

#if (DEBUGPRINT == terminal)
// Attach virtual serial terminal to Virtual Pin V1
WidgetTerminal terminal(V1);
#endif

#include <ESP8266mDNS.h>

#include "webpages.h"

#include "rrmail.h"

#include "rrtime.h"
#include "rrsettings.h"

#include "statistic.h"

#include "WemosRelay.h"

#include "rrfs.h"

unsigned int PulseCnt=0;
ESP8266WebServer server(80);
RRTime rrtime;
RRSettings rrsettings;
Statistic<unsigned int,60> PulsePerHour; //last hour
Statistic<unsigned int,24> PulsePerDay; //last day
const int led = 1;

WemosRelay relay;


RRFs rrfs(&server);

//Blynk
bool blynkEnabled;
bool pinToggle=false;
int blynkRelayPin;

void pinChanged(){
  PulseCnt++;
  pinToggle=true;
}

//To read Powersupply voltage:
ADC_MODE(ADC_VCC);
  
void setup() {
  relay.unset();
  DEBUGPRINT.begin(115200);
  delay(10);
  DEBUGPRINT.print("VCC:");DEBUGPRINT.println(ESP.getVcc());
  
  //pinMode(BUILTIN_LED, INPUT); 
  //pinMode(BUILTIN_LED, OUTPUT); 

  rrsettings.load("switch");  //Get the settings;
  const char* c= rrsettings.get("url").c_str();
  relay.setUrl(c);
  blynkRelayPin=(int)rrsettings.getLong("blynkPin");

  
  // We start by connecting to a WiFi network
  DEBUGPRINT.println();
  rrsettings.load("wifi");  //Get the settings;
  DEBUGPRINT.print("Connecting to ");
  DEBUGPRINT.println(rrsettings.get("ssid"));
  String  ssid=rrsettings.get("ssid");
  String pass=rrsettings.get("pass");
  WiFi.mode(WIFI_STA); //default: join the WIFI
  WiFi.begin(ssid.c_str(), pass.c_str());
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUGPRINT.print(WiFi.status());
    if(millis()>15000){
      DEBUGPRINT.println("\nERROR! No Connection!\nStarting AccessPoint:");
        WiFi.disconnect();
        delay(100);
        WiFi.mode(WIFI_AP); //create Access Pint
        WiFi.softAP("ESP8266", "happyornot");
        IPAddress myIP = WiFi.softAPIP();
        DEBUGPRINT.println(myIP);
        DEBUGPRINT.println("------------");
        break;
      }
  }
 
    DEBUGPRINT.println("");
    DEBUGPRINT.println("WiFi connected");  
    DEBUGPRINT.println("IP address: ");
    DEBUGPRINT.println(WiFi.localIP());
    rrsettings.load("device");
    String host=rrsettings.get("floor")+"."+rrsettings.get("room")+"."+rrsettings.get("name");
    if (!MDNS.begin(host.c_str())) {
      DEBUGPRINT.println("Error setting up MDNS responder!");
    }
    DEBUGPRINT.printf("Ready! Open http://%s.local in your browser\n", host.c_str());
   // Add service to MDNS-SD
   MDNS.addService("http", "tcp", 80);
  //Telegramm:
  //bot = new TelegramBOT(rrsettings.settings.BotToken, rrsettings.settings.BotName, rrsettings.settings.BotUsername);
  //bot->begin();      // launch Bot functionalities
  //if(rrsettings.settings.BotEnable){
  //  bot->sendMessage(rrsettings.settings.BotChatId, "ESPLogger Tool ist Online\n", "");
  //}

  if(SPIFFS.begin()){
    DEBUGPRINT.println("Filesystem started");
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DEBUGPRINT.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
    }
    DEBUGPRINT.printf("\n");
  }else{
    DEBUGPRINT.println("Filesystem failed");  
  }

  //Attach a GPIO Interrrupt
   attachInterrupt(D2, pinChanged, CHANGE);

   //enalbe Periodic sync to NTP:
  //Setup ntp sync
  DEBUGPRINT.println("Checking NTP Service");
  rrsettings.load("ntp");
  if(rrsettings.getBool("enabled")){
    setSyncProvider(rrtime.getTime);
    if(rrsettings.getLong("interval")<1){rrsettings.set("interval",1);} //not smaler than 1h
    setSyncInterval((rrsettings.getLong("interval"))*3600); //interval is given in h
    DEBUGPRINT.println(String("NTP Setup: Every ")+rrsettings.get("interval") +" h");
  }
  DEBUGPRINT.println("Checking NTP Service ended");
    server.on("/", handleRoot );
    server.on("/index/", handleRoot );
    server.on("/setup/", handleSetup );
    server.on("/ntp/sync/",[]() { server.send (200, "application/json", "{\"sync\":\"true\"}"); 
                                  rrtime.getTime(); });
    server.on("/reboot", []() {
    server.send( 200, "application/json", "{\"rebooting\":\"true\"}" );
    ESP.restart();
  });
  server.on("/get/wifilist", []() {
    server.send (200, "text/html",rrsettings.wifiList());
  });
  server.on("/graph_hour.svg", drawGraphHour);
  server.on("/graph_day.svg", drawGraphHour);
  server.on("/hour.csv", []() {
    server.send (200, "text/csv",PulsePerHour.toCsv());
  });
  server.on("/day.csv", []() {
    server.send (200, "text/csv",PulsePerDay.toCsv());
  });
  server.on("/get/day/stat", []() {
   char tmp[60];
    snprintf ( tmp, 20,"{day_mean:%d,day_var:%d,day_stdDev:%d}",PulsePerDay.meanf(),PulsePerDay.variancef(),PulsePerDay.stdDevf());
    server.send (200, "text/plain",tmp);
  });
  server.on("/get/count", []() {
    char tmp[25];
    snprintf ( tmp, 20,"{count:%d}",PulseCnt);
    server.send (200, "text/plain",tmp);
  });
  server.on("/get/temperature", []() {
    char tmp[20];
    snprintf ( tmp, 20,"{TDOD:temperature:%f}",PulseCnt);
    server.send (200, "text/plain",tmp);
  });
  server.on("/get/humidity", []() {
    char tmp[20];
    snprintf ( tmp, 20,"{TODO:humidity:%f}",PulseCnt);
    server.send (200, "text/plain",tmp);
  });
  server.on("/get/switch", []() {
    char tmp[20];
    snprintf ( tmp, 20,"{switch:%d}",relay.state());
    server.send (200, "text/plain",tmp);
  });
  server.on("/set/switch/1", []() {
    relay.set();
    char tmp[20];
    snprintf ( tmp, 20,"{switch:%d}",relay.state());
    server.send (200, "text/plain",tmp);
  });
  server.on("/set/switch/0", []() {
    relay.unset();
    char tmp[20];
    snprintf ( tmp, 20,"{switch:%d}",relay.state());
    server.send (200, "text/plain",tmp);
  });
  //NOW DONE IN RRFS-Class server.onNotFound(handleNotFound);
  server.begin();
/*
RRMail mail;
  if(mail.sendMail("r.raekow@gmail.com;roman.raekow@gmx.de;wurzelpost@gmail.com","Test","Leerer inhalt der <h2>HTML</h2>enthält")) DEBUGPRINT.println("Email sent");
      else DEBUGPRINT.println("Email failed");
*/

  ///////////////////////////////////////////////////
  /// BLYNK
  ///////////////////////////////////////////////////
 
  rrsettings.load("blynk");  //Get the settings;
  blynkEnabled = rrsettings.getBool("enabled",true);
  if(blynkEnabled){

    DEBUGPRINT.print("\nBlynk auth:");
    Serial.println(rrsettings.get("auth").c_str());
    
    strcpy(blynkAuth,rrsettings.get("auth").c_str());
    Serial.print("\n\t");
    Serial.println(blynkAuth);
    Blynk.config(blynkAuth);  // Here your Arduino connects to the Blynk Cloud.
    DEBUGPRINT.print("Connecting to Blynk");
    Blynk.connect(30000);
    //while(Blynk.connect() == false){
      ;//whait
    //}
    //Blynk.begin(blynkAuth,ssid.c_str(),pass.c_str());
    if(Blynk.connected())  {
       DEBUGPRINT.println(" ok");
    }else{
      DEBUGPRINT.println(" failed");    
    }
    
  }
}


// This is called for all virtual pins, that don't have BLYNK_WRITE handler
BLYNK_WRITE_DEFAULT() {
  Serial.print("Blynk input V");
  Serial.print(request.pin);
  Serial.println(":");
  // Print all parameter values
  for (auto i = param.begin(); i < param.end(); ++i) {
    Serial.print("* ");
    Serial.println(i.asString());
  }

  if(blynkRelayPin == request.pin){
    Serial.println("Pin match!!!");
      if(param.asInt()){
        relay.set();
        Serial.println("Pin Set");
      }else{
        relay.unset();
        Serial.println("Pin Unset");
      }
    }
}

// This is called for all virtual pins, that don't have BLYNK_READ handler
BLYNK_READ_DEFAULT() {
  // Generate random response
  int val = random(0, 100);
  Serial.print("Blynk output V");
  Serial.print(request.pin);
  Serial.print(": ");
  Serial.println(val);
  Blynk.virtualWrite(request.pin, val);
}

int value = 0;
unsigned long previousMillis = 0;
unsigned long interval = 1000;
byte lastLogMin=-1;

void loop() {
  
  if(blynkEnabled && Blynk.connected()){
    Blynk.run();
  }

  if(pinToggle){
    pinToggle=false;
    relay.setState(digitalRead(D2));
  }
  
  unsigned int currentMillis =0;
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;
  }

  if(lastLogMin != minute()){
    lastLogMin=minute();
    relay.unset();
    unsigned int PulseCntBuff =PulseCnt;
    PulseCnt=0;
    PulsePerHour.setData(PulseCntBuff,minute());
    //if 24h have elapsed: reset the value from 24h ago
    if(minute()==0){
      PulsePerDay.resetData(hour());
    }
    PulsePerDay.sumData(PulseCntBuff,hour());
  }
  server.handleClient();
  
  //run the relay
  if(relay.popToggled()){
    Blynk.virtualWrite(blynkRelayPin, relay.state());
  }
}







