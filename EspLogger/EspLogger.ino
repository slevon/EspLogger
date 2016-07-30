/*
 *  This sketch creates a IOT Thing
 */

#define VERSION "0.1"

#include <TimeLib.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
//Telegram
#include <WiFiClientSecure.h>
#include <TelegramBot.h>
#include "rrmail.h"

#include "rrtime.h"
#include "rrapsettings.h"

unsigned int cnt=0;
unsigned int buffer[100]; //Buffer for the last 100 cnt s
ESP8266WebServer server(80);
RRTime rrtime;
RRApSettings rrsettings;
const int led = 1;


// Initialize Telegram BOT
TelegramBOT* bot;

void pinChanged(){
  cnt++;
  //digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));  
}

void setup() {
  Serial.begin(115200);
  delay(10);
  //pinMode(BUILTIN_LED, INPUT); 
  //pinMode(BUILTIN_LED, OUTPUT); 
  
  rrsettings.restore();  //Get the settings;
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(rrsettings.settings.ssid);

  WiFi.mode(WIFI_STA); //default: join the WIFI
  WiFi.begin(rrsettings.settings.ssid, rrsettings.settings.pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.status());
    if(millis()>15000){
      Serial.println("\nERROR! No Connection!\nStarting AccessPoint:");
        /* You can remove the password parameter if you want the AP to be open. */
        WiFi.disconnect();
        delay(100);
        WiFi.mode(WIFI_AP); //create Access Pint
        WiFi.softAP("ESP8266_2", "happyornot");
        IPAddress myIP = WiFi.softAPIP();
        Serial.println(myIP);
        Serial.println("------------");
        break;
      }
  }

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  //Telegramm:
  //bot = new TelegramBOT(rrsettings.settings.BotToken, rrsettings.settings.BotName, rrsettings.settings.BotUsername);
  //bot->begin();      // launch Bot functionalities
  //if(rrsettings.settings.BotEnable){
  //  bot->sendMessage(rrsettings.settings.BotChatId, "ESPLogger Tool ist Online\n", "");
  //}
  
  //Attach a GPIO Interrrupt
   attachInterrupt(2, pinChanged, RISING);

   //enalbe Periodic sync to NTP:
  //Setup ntp sync
  Serial.println("Checking NTP Service");
  if(rrsettings.settings.ntpEnable){
    setSyncProvider(rrtime.getTime);
    if(rrsettings.settings.ntpInterval<1){rrsettings.settings.ntpInterval = 1;} //not smaler than 1h
    setSyncInterval((rrsettings.settings.ntpInterval)*3600); //interval is given in h
    Serial.println("NTP Setup succeed: Every "+String(rrsettings.settings.ntpInterval) +" h");
  }
  Serial.println("Checking NTP Service ended");
    server.on("/", handleRoot );
    server.on("/index", handleRoot );
    server.on("/wifi", setupWifi );
    server.on("/ntp", setupNtp );
    server.on("/email",setupEmail);
  server.on("/telegram", setupTelegram);
  server.on("/reboot", []() {
    if(millis()>60000){
      server.send (200, "text/html",htmlHeader()+"<h2>Ich starte neu. Bitte warten.</h2><script> setTimeout(function(){;window.location.assign('../index');},15000);</script>"+htmlFooter(ESP.getFreeHeap()));
    }else{
      server.send (200, "text/html",htmlHeader()+"<h2>Reboot abgebrochen. ESP uptime weniger als eine Minute. Bitte besuche die <a href='../index'>Indexseite</a></h2>"+htmlFooter(ESP.getFreeHeap()));
    }
    ESP.restart();
  });
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
/*
RRMail mail;
  if(mail.sendMail("r.raekow@gmail.com;roman.raekow@gmx.de;wurzelpost@gmail.com","Test","Leerer inhalt der <h2>HTML</h2>enthält")) Serial.println("Email sent");
      else Serial.println("Email failed");
*/
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





#define STATUS_REBOOT "<div class='msgbox'>Einstellungen gespeichert. Nach <a href='../reboot'>Reboot</a> werden die Änderungen wirksam.</div>"




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
      "<h1>Hallo ich bin <em>%s</em></h1>\
      <p>Uptime: %02d:%02d:%02d</p>\
      <p>Zeit: %s</p>\
      <img src=\"/graph.svg\" />\
      <p>Firmware: %s  vom %s %s</p>\
      <p>Heap: %d</p>\
      <p>Flash size: %d</p>",
    rrsettings.settings.deviceName,
    hr, min % 60, sec % 60,rrtime.dateTimeString().c_str(),VERSION ,__DATE__,__TIME__,ESP.getFreeHeap(),ESP.getFlashChipSize()
  );
  server.send ( 200, "text/html",  htmlHeader() +temp + htmlFooter(ESP.getFreeHeap()));
}


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void handleNotFound() {
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


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void setupWifi() {
  //Check if we have a transmission of arguments, if so: Check if we can apply it to the settings:
  String statusString ="";
  if(server.args() > 0){
    //default to false. (http does not send checkboxes if false, otherwise will be rest
    rrsettings.settings.enableHttpSend = false;
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
      }else if(server.argName (i) == "deviceName"){
        sprintf(rrsettings.settings.deviceName, RRApSettings::urldecode(server.arg(i)).c_str());
      }else if(server.argName (i) == "deviceFloor"){
        rrsettings.settings.deviceFloor = server.arg(i).toInt();
      }else if(server.argName (i) == "deviceId"){
        rrsettings.settings.deviceId = server.arg(i).toInt();
      }else if(server.argName (i) == "deviceRoom"){
        sprintf(rrsettings.settings.deviceRoom, RRApSettings::urldecode(server.arg(i)).c_str());
      }
    }
    statusString = STATUS_REBOOT;
    rrsettings.save();
    rrsettings.restore();
  }
  
    //The input fields need a dummy hidden filed to send a value if they are unchecked
  String html = htmlHeader()+"<h1><a href='#' onclick='rl();'>Einstellungen WLAN &#x21BB;</a></h1>"+statusString+
                 "<script>\
function getN(name){return document.getElementsByName(name)[0];}\n\
function tgl(name,ckb){getN(name).disabled=!getN(ckb).checked;}\n\
function a(){tgl('posturl','enablePost');tgl('postinterval','enablePost');}\n\
function rl(){location.reload();}\n\
</script>\n\
<form action='' method='post'>\n\
<table>\
<caption></caption>\
<tr>\
<td>SSID:</td>\
<td style='width:100%'> <select name='ssid'>"+rrsettings.wifiList()+"</select></td>\
</tr>\n\
<tr>\
<td>Passwort:</td>\
<td><input type='password'  maxlength='33' name='password' value='"+rrsettings.settings.pass+"'></td>\
</tr>\
<tr>\
<td>HTTP&nbsp;Put:</td><td>\
<input type='checkbox' id='enablePut' name='enablePost' onclick='a();' value='1' " + (rrsettings.settings.enableHttpSend?"checked":"") + " /><label for='enablePut'>Aktiv</label></td>\
</tr>\
<tr>\n\
<td></td>\
<td>URL: <input " + (rrsettings.settings.enableHttpSend?"":"disabled") + " type='text' placeholder='e.g.: http://sever.com/log.php?id={id}&cnt={cnt}&tmp={temp}' maxlength='"+RRAPSETTINGS_DOUBLE_LONG_STRING+"' name='posturl' value='" + rrsettings.settings.httpSendUrl + "' />\
<ul><li>{cnt}: Count</li><li>{temp}: Temperatur</li><li>{hum}: Luftfeuchte</li><li>{time}: Uhrzeit</li><li>{id}: DeviceId</li></ul>\
<br/>Interval [min]: <input " + (rrsettings.settings.enableHttpSend?"":"disabled") + " type='number' step='1' name='postinterval' value='"+rrsettings.settings.httpSendInterval+"'>\
</td>\
</tr>\n\
<tr>\
<td>Device Id</td><td>\
<input type='number' step='1' min='0' name='deviceId' value='"+rrsettings.settings.deviceId+"' /></td>\
</tr>\n\
<tr>\
<td>Device Name</td><td>\
<input type='text' name='deviceName' value='"+rrsettings.settings.deviceName+"' placeholder='z.B. Flur-Licht' maxlength='"+RRAPSETTINGS_SHORT_STRING+"' /></td>\
</tr>\n\
<tr>\
<td>Device Raum</td><td>\
<input type='text' name='deviceRoom' value='"+rrsettings.settings.deviceRoom+"' placeholder='DG' maxlength='"+RRAPSETTINGS_SHORT_STRING+"' /></td>\
</tr>\n\
<tr>\
<td>Device&nbsp;Stockwerk</td><td>\
<input type='number'  step='1'  name='deviceFloor' value='"+rrsettings.settings.deviceFloor+"' /></td>\
</tr>\n\
<tr>\
<td></td><td><button type='submit'>&#x2714; Speichern</button><td>\
</tr>\
</table>\
</form>"+htmlFooter(ESP.getFreeHeap());
 server.send( 200, "text/html", html );
}


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void setupNtp() {
  //Check if we have a transmission of arguments, if so: Check if we can apply it to the settings:
  String statusString ="";
  if(server.args() > 0){
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.println(server.argName ( i ) + ": " + server.arg ( i ));
      if(server.argName (i) == "enableNtp"){
        rrsettings.settings.ntpEnable=server.arg (i).toInt();
      }
      if(server.argName (i) == "ntpInterval"){
        int newInterval=server.arg (i).toInt();
        if(newInterval > 0){
          rrsettings.settings.ntpInterval=newInterval;
        }
      }
    }
    
    statusString =STATUS_REBOOT;
    rrsettings.save(); 
    rrsettings.restore();//Read back
  }

  unsigned long last=(millis() - rrtime.getLastSync())/1000;
  byte s=last%60;
  byte m=s/60; 
  byte h=m/60;
  
  //The input fields need a dummy hidden filed to send a value if they are unchecked
  String html = htmlHeader()+"<h1><a href='#' onclick='rl();'>Einstellungen NTP</a></h1>"+statusString+
 "<p>Aktivieren sie diese Funktion um die Uhr immer auktuell zu halten (Benötigt eine Internetverbindung).</p>\
 <p>Aktuelle Zeit:"+rrtime.dateTimeString()+"</p>\
 <p>NTP Status:"+rrtime.getStatus()+"</p>\
 <p>Letzter Sync:"+h+"h "+m+"m "+s+"s</p>\
 <p>Sync Interval:"+rrsettings.settings.ntpInterval+"h</p>\
 <form action='' method='post'>\n\
 <table>\
  <caption></caption>\
  <tr>\
  <td>NTP</td>\
  <td><input type='hidden' value='0' name='enableNtp'/>\
  <input type='checkbox' id='enableNtp' name='enableNtp' onclick='a();' value='1' " + (rrsettings.settings.ntpEnable?"checked":"") + " /><label for='enableNtp'>Aktiv</label></td>\
  </tr>\
  <tr>\
  <td>Update</td>\
  <td><input type='number' step='1'  min='1' name='ntpInterval' value='" + rrsettings.settings.ntpInterval + "'  /> Stunden</td>\
  </tr>\
  <tr>\
  <td></td><td><button type='submit'>&#x2714; Speichern</button><td>\
  </tr>\
  </table>\
  </form>"+htmlFooter(ESP.getFreeHeap());
 server.send ( 200, "text/html", html );
}


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void setupEmail() {
  //Check if we have a transmission of arguments, if so: Check if we can apply it to the settings:
  String statusString ="";
  if(server.args() > 0){
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.println(server.argName ( i ) + ": " + server.arg ( i ));
      if(server.argName (i) == "mailEnable"){
        rrsettings.settings.mailEnable=server.arg (i).toInt();
      }
      if(server.argName (i) == "mailReceiver"){
           sprintf(rrsettings.settings.mailReceiver, RRApSettings::urldecode(server.arg(i)).c_str());
        }
      }
    
    statusString =STATUS_REBOOT;
    rrsettings.save(); 
    rrsettings.restore();//Read back
  }

  unsigned long last=(millis() - rrtime.getLastSync())/1000;
  byte s=last%60;
  byte m=s/60; 
  byte h=m/60;
  
  //The input fields need a dummy hidden filed to send a value if they are unchecked
  String html = htmlHeader()+"<h1><a href='#' onclick='rl();'>Einstellungen Email</a></h1>"+statusString+
 "\
 <form action='' method='post'>\n\
 <table>\
  <caption></caption>\
  <tr>\
  <td>Email</td>\
  <td><input type='hidden' value='0' name='mailEnable'/>\
  <input type='checkbox' id='mailEnable' name='mailEnable' value='1' " + (rrsettings.settings.mailEnable?"checked":"") + " /><label for='mailEnable'>Aktiv</label></td>\
  </tr>\
  <tr>\
  <td>Email</td>\
  <td><input type='text' name='mailReceiver' value='"+rrsettings.settings.mailReceiver+"' placeholder='a@b.com;c@d.com' maxlength='"+RRAPSETTINGS_DOUBLE_LONG_STRING+"' title='Mehrere Emails mit \';\' trennen' /></td>\
  </tr>\
  <tr>\
  <td></td><td><button type='submit'>&#x2714; Speichern</button><td>\
  </tr>\
  </table>\
  </form>"+htmlFooter(ESP.getFreeHeap());
 server.send ( 200, "text/html", html );
}


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void setupTelegram() {
  //Check if we have a transmission of arguments, if so: Check if we can apply it to the settings:
  String statusString ="";
  if(server.args() > 0){
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.println(server.argName ( i ) + ": " + server.arg ( i ));
      if(server.argName (i) == "BotEnable"){
        rrsettings.settings.BotEnable=server.arg (i).toInt();
      }
      if(server.argName (i) == "BotToken"){
         sprintf(rrsettings.settings.BotToken,RRApSettings::urldecode(server.arg(i)).c_str());
      }
      if(server.argName (i) == "BotName"){
           sprintf(rrsettings.settings.BotName,RRApSettings::urldecode(server.arg(i)).c_str());
      }
      if(server.argName (i) == "BotUsername"){
           sprintf(rrsettings.settings.BotUsername,RRApSettings::urldecode(server.arg(i)).c_str());
      }
      if(server.argName (i) == "BotChatId"){
           sprintf(rrsettings.settings.BotChatId,RRApSettings::urldecode(server.arg(i)).c_str());
      }
    }
    
    statusString = STATUS_REBOOT;
    rrsettings.save(); 
    rrsettings.restore();//Read back
  }
    //The input fields need a dummy hidden filed to send a value if they are unchecked
  String html = htmlHeader()+"<h1><a href='#' onclick='rl();'>Einstellungen Telegram</a></h1>"+statusString+
 "<p>Aktivieren sie diese Funktion um Meldungen über Telegramm auszutauschen.</p>\
 <form action='' method='post'>\n\
 <table>\
  <caption></caption>\
  <tr>\
  <td>Telegram</td>\
  <td><input type='hidden' value='0' name='BotEnable'/>\
  <input type='checkbox' id='BotEnable' name='BotEnable' value='1' " + (rrsettings.settings.BotEnable?"checked":"") + " /><label for='BotEnable'>Aktiv</label></td>\
  </tr>\
  <tr>\
  <td>Token</td>\
  <td><input type='text' name='BotToken' value='" + rrsettings.settings.BotToken + "'  /></td>\
  </tr>\
  <tr>\
  <td>Bot-Name</td>\
  <td><input type='text' name='BotName' value='" + rrsettings.settings.BotName + "'  /></td>\
  </tr>\
  <tr>\
  <td>User-Name</td>\
  <td><input type='text' name='BotUsername' value='" + rrsettings.settings.BotUsername + "'  /></td>\
  </tr>\
  <tr>\
  <td>Chat-ID</td>\
  <td><input type='text' name='BotChatId' value='" + rrsettings.settings.BotChatId + "'  /></td>\
  </tr>\
  <tr>\
  <td></td><td><button type='submit'>&#x2714; Speichern</button><td>\
  </tr>\
  </table>\
  </form>"+htmlFooter(ESP.getFreeHeap());
 server.send ( 200, "text/html", html );
  }


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
String htmlHeader(){
  return F("<html>\
  <head>\
  <meta charset='utf-8' http-equiv='refresh'/>\
  <title>ESP8266</title>\n\
  <style>\
  body { background-color: #fff; font-family: Tahoma, Helvetica, Sans-Serif; color: #07113A;  font-size: 20px; }\
  a{color:#162756;text-decoration:none;}\
  input,select { font-size: 22px; color:inherit; }\
  table{border-collapse: separate;\
     border: 2px solid #162756;\
  border-radius: 6px;\
  border-spacing: 0px;}\
  th, td {\
  padding: 5px;\
  text-align: left;\
  }\
  ul{margin:5px;}\
  input:not([type=checkbox]),select,table{width:100%;}\
  ul.sonarmenu{list-style: none;}\n\
  ul.sonarmenu li{display: inline;}\
  ul.sonarmenu a{position: relative;display: inline-block;color: #07113A;text-decoration: none;margin: 10px 20px;text-transform: uppercase;font-size: 22px;\
    letter-spacing: 2px;border-bottom: 2px solid transparent;}\
  ul.sonarmenu a:hover, ul.sonarmenu a:focus{outline: none;border-bottom: 2px solid #07113A;}\
  ul.sonarmenu a::before, ul.sonarmenu a:after{position: absolute;top: 50%;left: 50%;}\
  .msgbox{padding:10px; margin:10px;color:#fff;background:#07113A; border:2px solid #162756;border-radius: 6px;}\
  .msgbox a {color:#aaa;}\
  </style>\n\
  </head>\
  <body>\
  <ul class='sonarmenu'>\
  <li><a href='../index'>&#8962; Home</a></li>\
  <li><a href='../wifi'>&#x2699; WLAN/Http</a></li>\
  <li><a href='../email'>Email</a></li>\
  <li><a href='../ntp'>NTP</a></li>\
  <li><a href='../telegram'>Telegram</a></li>\
  <li><a href='../reboot'>Reboot</a></li>\
  </ul>\n\
  ");
  }
/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
String htmlFooter(long heap){
  return String("<p><small>Heap: ")+heap+" </small></p>\
     </body>\
    </html>";
  }



