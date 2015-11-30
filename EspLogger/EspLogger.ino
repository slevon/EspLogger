/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
#include <TimeLib.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

#include "rrtime.h"
#include "rrapsettings.h"

//const char* ssid     = "lamed";
//const char* password = "risJXPNY";
//const char* ssid     = "FP1";
//const char* password = "KatzeHundMaus";
const char ssid[] = "Besser geht es";  //  your network SSID (name)
const char password[] = "2005544074238576";       // your network password

const char* host = "10.42.01";
const char* message   = "ESP%20kann%20es";

unsigned int cnt=0;

unsigned int buffer[144]; //6*24h Einen Tag Puffer maximal, bei messwerten alle 10min

RRTime rrtime;
RRApSettings rrsettings;

void pinChanged(){
  cnt++;
}


void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.status());
    if(millis()>10000){
      Serial.println("ERROR! No Connection!");
      break;
      }
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Heap:");
  Serial.println(ESP.getFreeHeap());

  //Attach a GPIO Interrrupt
   attachInterrupt(2, pinChanged, RISING);
   
  setSyncProvider(rrtime.getTime);
  setSyncInterval(60*60*12); 

  //rrsettings.restoreSettings();
  //sprintf(rrsettings.settings.ssid,"Hallo Roman");
  //Serial.print(" Gesetzt:");
  //Serial.println(rrsettings.settings.ssid);
  //rrsettings.settings.ssid=String("Hallo Roman").c_str();
  //rrsettings.settings.ssid="geheim! Geheim";
  //rrsettings.saveSettings();
   
}

int value = 0;

void loop() {
  delay(5000);

  rrtime.digitalClockDisplay();
 /* ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/logger.php";
  //url += streamId;
  url += "?message=";
  url += message;
  url += "&value=";
  url += cnt;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(1000);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  Serial.print("Cnt:");
  Serial.println(cnt);
  cnt = 0;
  */
}

