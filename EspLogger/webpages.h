#ifndef WEBPAGES
#define WEBPAGES


#include "rrsettings.h"
#include "rrtime.h"
extern RRSettings rrsettings;
extern RRTime rrtime;
extern ESP8266WebServer server;
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Websites
//////////////////////////////////////////////////////////////////////////////////////////////////////
void handleRoot() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  rrsettings.load("device");
  snprintf ( temp, 400,
      "{\"name\":\"%s\",\
      \"uptime\":\"%02d:%02d:%02d\",\
      \"time\": \"%s\",\
      \"firmware\":\"%s %s %s\",\
      \"heap\":\"%d\",\
      \"flash-size\": %d",
    rrsettings.get("deviceName").c_str(),
    hr, min % 60, sec % 60,rrtime.dateTimeString().c_str(),VERSION ,__DATE__,__TIME__,ESP.getFreeHeap(),ESP.getFlashChipSize()
  );
  server.send ( 200, "text/html", temp );
}

/* NOW Done in RRFS-Class
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
}
*/

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void handleSetup() {
  Serial.println("Starting Setup Request");
   //Check if we have a transmission of arguments, if so: Check if we can apply it to the settings:
  String saved ="false";
  if(server.args() > 0){
    rrsettings.load(server.arg("rrsection"));
    if(rrsettings.saveServerArgs(server)){
      saved="true";
    }
  }
  rrsettings.load(server.arg("rrsection"));
  rrsettings.set("saved",JsonVariant(saved));
  server.send( 200, "application/json", rrsettings.toJson() );
  Serial.println("Ended Setup Request");
}


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void drawGraphHour() {
  /*
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
  */
//  server.send ( 200, "image/svg+xml", PulsePerHour.toSVG(400,150));
}
void drawGraphDay() {
  /*
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
  */
//  server.send ( 200, "image/svg+xml", PulsePerDay.toSVG(400,150));
}


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void setupTelegram() {/*
  //Check if we have a transmission of arguments, if so: Check if we can apply it to the settings:
  String statusString ="";
  if(server.args() > 0){
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      DEBUGPRINT.println(server.argName ( i ) + ": " + server.arg ( i ));
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
           sprintf(rrsettings.data["BotChatId",RRApSettings::urldecode(server.arg(i)).c_str());
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
 */
  }
  
#endif

