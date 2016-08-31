#ifndef WEBPAGES
#define WEBPAGES


#include "rrapsettings.h"
#include "rrtime.h"
extern RRApSettings rrsettings;
extern RRTime rrtime;
extern ESP8266WebServer server;

#define STATUS_REBOOT "<div class='msgbox'>Einstellungen gespeichert. Nach <a href='../reboot'>Reboot</a> werden die Änderungen wirksam.</div>"



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
  <li><a href='../wifi'>&#x2699; Setup/Http</a></li>\
  <li><a href='../email'>&#9993;Email</a></li>\
  <li><a href='../relay'>&#9716;Relais</a></li>\
  <!--<li><a href='../telegram'>Telegram</a></li>-->\
  <li><a href='../reboot'>Reboot</a></li>\
  </ul>\n\
  ");
  }
/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
String htmlFooter(long heap){
  return String("<p><small>Heap: ")+heap+" Bytes | VCC:"+ESP.getVcc()+" mV</small></p>\
     </body>\
    </html>";
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Websites
//////////////////////////////////////////////////////////////////////////////////////////////////////
void handleRoot() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 400,
      "<h1>Hallo ich bin <em>%s</em></h1>\
      <p>Uptime: %02d:%02d:%02d</p>\
      <p>Zeit: %s</p>\
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
<tr><td colspan='2'><h3>WLAN</h3></td></tr>\
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
<tr><td colspan='2'><h3>Device</h3></td></tr>\
<tr>\
<td>Id</td><td>\
<input type='number' step='1' min='0' name='deviceId' value='"+rrsettings.settings.deviceId+"' /></td>\
</tr>\n\
<tr>\
<td>Name</td><td>\
<input type='text' name='deviceName' value='"+rrsettings.settings.deviceName+"' placeholder='z.B. Deckenlampe' maxlength='"+RRAPSETTINGS_SHORT_STRING+"' /></td>\
</tr>\n\
<tr>\
<td>Raum</td><td>\
<input type='text' name='deviceRoom' value='"+rrsettings.settings.deviceRoom+"' placeholder='Flur' maxlength='"+RRAPSETTINGS_SHORT_STRING+"' /></td>\
</tr>\n\
<tr>\
<td>Stockwerk</td><td>\
<input type='number'  step='1'  name='deviceFloor' value='"+rrsettings.settings.deviceFloor+"' /></td>\
</tr>\n\
<tr><td colspan='2'><h3>NTP</h3></td></tr>\
<tr>\
  <td>NTP verwenden</td>\
  <td><input type='hidden' value='0' name='enableNtp'/>\
  <input type='checkbox' id='enableNtp' name='enableNtp' onclick='a();' value='1' " + (rrsettings.settings.ntpEnable?"checked":"") + " /><label for='enableNtp'>Aktiv</label></td>\
  </tr>\
  <tr>\
  <td>NTP-Update</td>\
  <td><input type='number' step='1'  min='1' name='ntpInterval' value='" + rrsettings.settings.ntpInterval + "'  /> Stunden</td>\
  </tr>\
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
// RELAY
/////////////////////////////////////////////////////////////////////////////////////
void setupRelay() {
  //Check if we have a transmission of arguments, if so: Check if we can apply it to the settings:
  String statusString ="";
  if(server.args() > 0){
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.println(server.argName ( i ) + ": " + server.arg ( i ));
      if(server.argName (i) == "relayEnable"){
        rrsettings.settings.relayEnable=server.arg (i).toInt();
      }
      if(server.argName (i) == "relayChangeUrl"){
           sprintf(rrsettings.settings.relayChangeUrl, RRApSettings::urldecode(server.arg(i)).c_str());
        }
      }
    
    statusString =STATUS_REBOOT;
    rrsettings.save(); 
    rrsettings.restore();//Read back
  }
  
  //The input fields need a dummy hidden filed to send a value if they are unchecked
  String html = htmlHeader()+"<h1><a href='#' onclick='rl();'>Einstellungen Relais</a></h1>"+statusString+
 "\
 <form action='' method='post'>\n\
 <table>\
  <caption></caption>\
  <tr>\
  <td>Relais</td>\
  <td><input type='hidden' value='0' name='relayEnable'/>\
  <input type='checkbox' id='relayEnable' name='relayEnable' value='1' " + (rrsettings.settings.relayEnable?"checked":"") + " /><label for='relayEnableble'>Aktiv</label></td>\
  </tr>\
  <tr>\
  <td>Direkt URL</td>\
  <td>http://<input type='text' name='relayChangeUrl' value='"+rrsettings.settings.relayChangeUrl+"' placeholder='192.168.0.1/set/{state}' maxlength='"+RRAPSETTINGS_LONG_STRING+"' title='Zusäztlich direkt aufgerufene URL' />\
   <ul><li>{state}: Aktueller Zustand des Relais</li></ul>\
   </td>\
</tr>\n\
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
  
#endif

