/*
 *  This sketch creates a IOT Thing
 */
#include <TimeLib.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include "rrmail.h"

#include "rrtime.h"
#include "rrapsettings.h"

//const char* ssid     = "lamed";
//const char* password = "risJXPNY";
//const char* ssid     = "FP1";
//const char* password = "KatzeHundMaus";
const char ssid[] = "Besser geht es";  //  your network SSID (name)
const char password[] = "2005544074238576";       // your network password

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
      Serial.println("\n--->ERROR! No Connection!");
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

  //enalbe Periodic sync to NTP:
  //Setup ntp sync
  setSyncProvider(rrtime.getTime);
  setSyncInterval(60*60*12); 

  rrsettings.restore();
  //sprintf(rrsettings.settings.ssid,"Hallo Roman");
  //Serial.print(" Gesetzt:");
  //Serial.println(rrsettings.settings.ssid);
  //rrsettings.settings.ssid=String("Hallo Roman").c_str();
  //rrsettings.settings.ssid="geheim! Geheim";
  //rrsettings.saveSettings();

  server.on("/", handleRoot );
  server.on("/index", handleRoot );
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


RRMail mail;
  if(mail.sendMail("r.raekow@gmail.com","Test","Leer")) Serial.println("Email sent");
      else Serial.println("Email failed");

  
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
      "<h1>Hello from ESP8266!</h1>\
      <p>Uptime: %02d:%02d:%02d</p>\
      <img src=\"/graph.svg\" />\
      <p>Heap: %d</p>\
      <p>Flash size: %d</p>",
    hr, min % 60, sec % 60,ESP.getFreeHeap(),ESP.getFlashChipSize()
  );
  server.send ( 200, "text/html",  htmlHeader() +temp + htmlFooter(ESP.getFreeHeap()));
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
  String statusString ="";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.println(server.argName ( i ) + ": " + server.arg ( i ));
      if(server.argName (i) == "ssid"){
        sprintf(rrsettings.settings.ssid, RRApSettings::urldecode(server.arg (i)).c_str());
      }else if(server.argName (i) == "password"){
        sprintf(rrsettings.settings.pass, RRApSettings::urldecode(server.arg (i)).c_str());
      }else if(server.argName (i) == "enablePost"){
        rrsettings.settings.enableHttpSend=server.arg (i).toInt();
      }else if(server.argName (i) == "posturl"){
        sprintf(rrsettings.settings.httpSendUrl, RRApSettings::urldecode(server.arg(i)).c_str());
      }else if(server.argName (i) == "postinterval"){
        rrsettings.settings.httpSendInterval= server.arg (i).toInt();
      }
    }
  if(server.args() > 0){
    statusString = "<div style='padding:10px; margin:10px;color:#fff;background:#07113A; border:2px solid #162756;border-radius: 6px;'>Settings saved</div>";
    rrsettings.save();
    rrsettings.restore();//Read back
  }
  
  
  String html = htmlHeader()+statusString+"<form action='' method='post'>\
                 <table>\
                  <caption><h1><a href='#' onclick='rl();'>Einstellungen WLAN &#x21BB;</a></h1></caption>\
                  <tr>\
                  <td>SSID:</td>\
                  <td style='width:100%'> <select name='ssid'>"+rrsettings.wifiList()+"</select></td>\
                  </tr>\
                  <tr>\
                  <td>Passwort:</td>\
                  <td><input type='password'  maxlength='33' name='password' value='"+rrsettings.settings.pass+"'></td>\
                  </tr>\
                  <tr>\
                  <td>HTTP&nbsp;Put:</td>\
                  <td><input type='checkbox' id='enablePut' name='enablePost' onclick='a();' value='1' " + (rrsettings.settings.enableHttpSend?"checked":"") + " /><label for='enablePut'>Aktiv</label></td>\
                  </tr>\
                  <tr>\
                  <td></td>\
                  <td>URL: <input " + (rrsettings.settings.enableHttpSend?"":"disabled") + " type='text' placeholder='e.g.: http://sever.com/log.php?cnt={cnt}&tmp={temp}' maxlength='100' name='posturl' value='" + rrsettings.settings.httpSendUrl + "' />\
                    <ul><li>{cnt}: Count</li><li>{temp}: Temperatur</li><li>{hum}: Luftfeuchte</li><li>{time}: Uhrzeit</li></ul>\
                    <br/>Interval [min]: <input " + (rrsettings.settings.enableHttpSend?"":"disabled") + " type='number' step='1' name='postinterval' value='"+rrsettings.settings.httpSendInterval+"'>\
                  </td>\
                  </tr>\
                  <tr>\
                  <td></td><td><button type='submit'>&#x2714; Speichern und Reboot</button><td>\
                  </tr>\
                  </table>\
                  </form>"+htmlFooter(ESP.getFreeHeap());
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
  return String("<html>\
  <head>\
  <meta charset='utf-8' http-equiv='refresh'/>\
  <title>ESP8266</title>\
  <style>\
  body { background-color: #fff; font-family: Tahoma, Helvetica, Sans-Serif; color: #07113A;  font-size: 20px; }\
  a{color:#162756;text-decoration:none;}\
  input,select { font-size: 22px; color:inherit; }\
  table{border-collapse: separate;\
     border: 2px solid #162756;\
  border-radius: 6px;\
  border-spacing: 0px;}\
   \
  th, td {\
  padding: 5px;\
  text-align: left;\
  }\
  ul{margin:5px;}\
  input:not([type=checkbox]),select,table{width:100%;}\
  ul.sonarmenu{list-style: none;}\
  ul.sonarmenu li{display: inline;}\
  ul.sonarmenu a{position: relative;display: inline-block;color: #07113A;text-decoration: none;margin: 10px 20px;text-transform: uppercase;font-size: 22px;\
    letter-spacing: 2px;border-bottom: 2px solid transparent;}\
  ul.sonarmenu a:hover, ul.sonarmenu a:focus{outline: none;border-bottom: 2px solid #07113A;}\
  ul.sonarmenu a::before, ul.sonarmenu a:after{position: absolute;top: 50%;left: 50%;}\
  </style>\
  <script>\
  function getN(name){return document.getElementsByName(name)[0];}\
  function tgl(name,ckb){getN(name).disabled=!getN(ckb).checked;}\
  function a(){tgl('posturl','enablePost');tgl('postinterval','enablePost');}\
  function rl(){location.reload();}\
  </script>\
  </head>\
  <body>\
  <ul class='sonarmenu'>\
  <li><a href='../index'>&#8962; Home</a></li>\
  <li><a href='../setup'>&#x2699; WLAN</a></li>\
  <li><a href='../email'>Email</a></li>\
  <li><a href='../ntp'>NTP</a></li>\
  <li><a href='../telegramm'>Telegramm</a></li>\
  </ul>\
  ");
  }

String htmlFooter(long heap){
  return String("<p>Heap: ")+heap+" </p>\
     </body>\
    </html>";
  }

