/*
 *  This sketch creates a IOT Thing
 */
#include <TimeLib.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

#include "rrtime.h"
#include "rrapsettings.h"

const char* ssid     = "lamed";
const char* password = "risJXPNY";
//const char* ssid     = "FP1";
//const char* password = "KatzeHundMaus";
//const char ssid[] = "Besser geht es";  //  your network SSID (name)
//const char password[] = "2005544074238576";       // your network password

unsigned int cnt=0;
unsigned int buffer[100]; //Buffer for the last 100 cnt s
ESP8266WebServer server(80);
RRTime rrtime;
RRApSettings rrsettings;
const int led = 13;

void pinChanged(){
  cnt++;
}

void setup() {
  Serial.begin(115200);
  delay(10);
  // We start by connecting to a WiFi network
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

  //Setup ntp sync
  setSyncProvider(rrtime.getTime);
  setSyncInterval(60*60*12); 

  //rrsettings.restoreSettings();
  //sprintf(rrsettings.settings.ssid,"Hallo Roman");
  //Serial.print(" Gesetzt:");
  //Serial.println(rrsettings.settings.ssid);
  //rrsettings.settings.ssid=String("Hallo Roman").c_str();
  //rrsettings.settings.ssid="geheim! Geheim";
  //rrsettings.saveSettings();

  server.on("/", handleRoot );
  server.on("/setup", setupWifi );
  server.on("/graph.svg", drawGraph);
  server.on("/get/count", []() {
    char temp[20];
    snprintf ( temp, 20,"{count:%d}",cnt);
    server.send (200, "text/plain",temp);
  });
  server.on("/get/temperature", []() {
    char temp[20];
    snprintf ( temp, 20,"{TDOD:temperature:%f}",cnt);
    server.send (200, "text/plain",temp);
  });
  server.on("/get/humidity", []() {
    char temp[20];
    snprintf ( temp, 20,"{TODO:humidity:%f}",cnt);
    server.send (200, "text/plain",temp);
  });
  server.onNotFound(handleNotFound);
  server.begin();
}

int value = 0;

void loop() {
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

  server.handleClient();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
// Websites
//////////////////////////////////////////////////////////////////////////////////////////////////////
void handleRoot() {
  digitalWrite ( led, 1 );
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 400,
  "<html>\
    <head>\
      <meta http-equiv='refresh' content='5'/>\
      <title>ESP8266 Demo</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>\
      <h1>Hello from ESP8266!</h1>\
      <p>Uptime: %02d:%02d:%02d</p>\
      <img src=\"/graph.svg\" />\
      <p>Heap: %d</p>\
      <p>Flash size: %d</p>\
    </body>\
  </html>",
    hr, min % 60, sec % 60,ESP.getFreeHeap(),ESP.getFlashChipSize()
  );
  server.send ( 200, "text/html", temp );
  digitalWrite ( led, 0 );
}

void handleNotFound() {
  digitalWrite ( led, 1 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( led, 0 );
}


void setupWifi() {
  //Check if we have a transmission of arguments, if so: Check if we can apply it to the settings:
  boolean settingsUpdated =false;
  //for ( uint8_t i = 0; i < server.args(); i++ ) {
  //  if(server.argName ( i ) ==  + ": " + server.arg ( i ) + "\n";
  //}
  
  String html = "<html>"
                "<head>"
                "<meta charset='utf-8' http-equiv='refresh' content='5'/>"
                "<title>ESP8266 Demo</title>"
                "<style>"
                "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }"
                "input { font-size: 30px }"
                "table, th, td {"
                "border: 1px solid black;"
                "border-collapse: collapse;"
                "font-size: 30px;}"
                "th, td {"
                "padding: 5px;"
                "text-align: left;}"
                "</style>"
                "</head>"
                "<body>"
                "<table style='width:100%'>"
                "<caption><h1 style='font-size:40px'><u>Einstellungen WiFi</u></h1></caption>"
                "<tr>"
                "<td><p>SSID:</p></td>"
                "<td>" + rrsettings.wifiList() + "</td>"
                "</tr>"
                "<tr>"
                "<td><p>Passwort:</p></td>"
                "<td><input type='password' size='40' id='password' name='password'></td>"
                "</tr>"
                "<tr>"
                "<td><p>IP:</p></td>"
                "<td><input type='text' size='40' id='ip' name='ip'></td>"
                "</tr>"
                "<tr>"
                "<td><p>Gateway:</p></td>"
                "<td><input type='text' size='40' id='gateway' name='gateway'></td>"
                "</tr>"
                "<tr>"
                "<td><p>Subnetz:</p></td>"
                "<td><input type='text' size='40' id='subnet' name='subnet'></td>"
                "</tr>"
                "<tr>"
                "<td></td><td text-align:='center'><form action='/setup' method='get'><button type='submit' id='update' onClick=location.href='/search_WIFIs'>Suche WIFIs</button></form><form action='/save_reboot' method='get'><button type='button' id='save' onClick=location.href='/save_reboot'>Speichern und Reboot</button></form><td>"
                "</tr>"
                "</table>"
                "</body>"
                "</html>";

 server.send ( 200, "text/html", html );
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x+= 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    sprintf(temp, "<text y=\"0\" x=\"%d\" >%d</text>\n", x, x);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send ( 200, "image/svg+xml", out);
}


String htmlHeader(){
  
  }

