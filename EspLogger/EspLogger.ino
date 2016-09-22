/*
 *  This sketch creates a IOT Thing
 */

#define VERSION "0.1"


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

#include <ESP8266mDNS.h>

#include "webpages.h"

#include "rrmail.h"

#include "rrtime.h"
#include "rrsettings.h"

#include "statistic.h"

#include "WemosRelay.h"

#include "rrfs.h"

unsigned int PulseCnt=0;
unsigned int buffer[100]; //Buffer for the last 100 cnt s
ESP8266WebServer server(80);
RRTime rrtime;
RRSettings rrsettings;
Statistic<unsigned int,60> PulsePerHour; //last hour
Statistic<unsigned int,24> PulsePerDay; //last day
const int led = 1;

WemosRelay relay;


RRFs rrfs(&server);

void pinChanged(){
  PulseCnt++;
  //digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));  
}

//To read Powersupply voltage:
ADC_MODE(ADC_VCC);
  
void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.print("VCC:");Serial.println(ESP.getVcc());


  relay.set();
  delay(500);
  relay.unset();
  //pinMode(BUILTIN_LED, INPUT); 
  //pinMode(BUILTIN_LED, OUTPUT); 
  
  rrsettings.load("relay");  //Get the settings;
  const char* c= String(rrsettings.data["relayChangeUrl"].asString()).c_str();
  relay.setUrl(c);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  rrsettings.load("wifi");  //Get the settings;
  Serial.println(rrsettings.data["ssid"].asString());

  WiFi.mode(WIFI_STA); //default: join the WIFI
  WiFi.begin(rrsettings.data["ssid"].asString(), rrsettings.data["pass"].asString());
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.status());
    if(millis()>15000){
      Serial.println("\nERROR! No Connection!\nStarting AccessPoint:");
        /* You can remove the password parameter if you want the AP to be open. */
        WiFi.disconnect();
        delay(100);
        WiFi.mode(WIFI_AP); //create Access Pint
        WiFi.softAP("ESP8266", "happyornot");
        IPAddress myIP = WiFi.softAPIP();
        Serial.println(myIP);
        Serial.println("------------");
        break;
      }
  }
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    rrsettings.load("device");
    String host=rrsettings.data["deviceRoom"].asString()+String(".")+rrsettings.data["deviceName"].asString();
    if (!MDNS.begin(host.c_str())) {
      Serial.println("Error setting up MDNS responder!");
    }
    Serial.printf("Ready! Open http://%s.local in your browser\n", host.c_str());
   // Add service to MDNS-SD
   MDNS.addService("http", "tcp", 80);
  //Telegramm:
  //bot = new TelegramBOT(rrsettings.settings.BotToken, rrsettings.settings.BotName, rrsettings.settings.BotUsername);
  //bot->begin();      // launch Bot functionalities
  //if(rrsettings.settings.BotEnable){
  //  bot->sendMessage(rrsettings.settings.BotChatId, "ESPLogger Tool ist Online\n", "");
  //}

  if(SPIFFS.begin()){
    Serial.println("Filesystem started");
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
    }
    Serial.printf("\n");
  }else{
    Serial.println("Filesystem failed");  
  }

  //Attach a GPIO Interrrupt
   attachInterrupt(2, pinChanged, RISING);

   //enalbe Periodic sync to NTP:
  //Setup ntp sync
  Serial.println("Checking NTP Service");
  rrsettings.load("ntp");
  if(rrsettings.data["ntpEnable"]){
    setSyncProvider(rrtime.getTime);
    if(rrsettings.data["ntpInterval"]<1){rrsettings.data["ntpInterval"] = 1;} //not smaler than 1h
    setSyncInterval((rrsettings.data["ntpInterval"].as<long>())*3600); //interval is given in h
    Serial.println(String("NTP Setup: Every ")+rrsettings.data["ntpInterval"].asString() +" h");
  }
  Serial.println("Checking NTP Service ended");
    server.on("/", handleRoot );
    server.on("/index", handleRoot );
    server.on("/setup", handleSetup );
    server.on("/ntp/sync/",[]() { server.send (200, "application/json", "{\"sync\":\"true\"}"); 
                                  rrtime.getTime(); });
    server.on("/reboot", []() {
    server.send( 200, "application/json", "{\"rebooting\":\"true\"}" );
    ESP.restart();
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
  server.on("/get/relay", []() {
    char tmp[20];
    snprintf ( tmp, 20,"{relay:%d}",relay.state());
    server.send (200, "text/plain",tmp);
  });
  server.on("/set/relay/1", []() {
    relay.set();
    char tmp[20];
    snprintf ( tmp, 20,"{relay:%d}",relay.state());
    server.send (200, "text/plain",tmp);
  });
  server.on("/set/relay/0", []() {
    relay.unset();
    char tmp[20];
    snprintf ( tmp, 20,"{relay:%d}",relay.state());
    server.send (200, "text/plain",tmp);
  });
  //NOW DONE IN RRFS-Class server.onNotFound(handleNotFound);
  server.begin();
/*
RRMail mail;
  if(mail.sendMail("r.raekow@gmail.com;roman.raekow@gmx.de;wurzelpost@gmail.com","Test","Leerer inhalt der <h2>HTML</h2>enthält")) Serial.println("Email sent");
      else Serial.println("Email failed");
*/
}

int value = 0;
unsigned long previousMillis = 0;
unsigned long interval = 1000;
byte lastLogMin=-1;
void loop() {
  unsigned int currentMillis =0;
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

   //TODO 
   
  }

  if(second() == 0 && lastLogMin != minute()){
    lastLogMin=minute();
    
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
  
  
  //TODODO   relay.run();
  //yield();
}







