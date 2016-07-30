
#include "rrmail.h"

RRMail::RRMail(){
  
  
}


RRMail::~RRMail(){
  
  
}

byte RRMail::sendMail(String to, String subject, String body){   
  byte thisByte = 0;
  byte respCode;
   Serial.println(F("STARTING EMail:"));
  if(client.connect("mail.smtp2go.com",2525) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending hello"));
  client.println("EHLO mail.smtp2go.com");
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending User"));
  client.println("YXJkdWlub3Jy");
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending Password"));
  client.println("MDgwOTE5ODQ=");
 
  if(!eRcv()) return 0;
 
// change to your email address (sender)
  Serial.println(F("Sending From"));
  client.println("MAIL From: r.raekow@gmail.com");
  if(!eRcv()) return 0;
 
// change to recipient address
  
  //Split Receipeints by ';'
  int lastTo=0;
  String recpt;
  while(to.indexOf(';',lastTo) != -1){
    Serial.print(F("Sending To: "));
    recpt=to.substring(lastTo,to.indexOf(';',lastTo));
    Serial.println(recpt);
    client.println("RCPT To: "+recpt);
    lastTo = to.indexOf(';',lastTo) +1;
  }
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending DATA"));
  client.println("DATA");
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending email"));
 
 //Split Receipeints by ';'
  lastTo=0;
  while(to.indexOf(';',lastTo) != -1){
    Serial.print(F("Sending Header To: "));
    recpt=to.substring(lastTo,to.indexOf(';',lastTo));
    Serial.println(recpt);
    client.println("To: "+recpt);
    lastTo = to.indexOf(';',lastTo) +1;
  }
 
// change to your address
  client.println("From: ESP8266 r.raekow@gmail.com");
 
  client.println("Subject: "+subject+"\r\n");
 
  client.println(body);
 
  client.println(".");
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending QUIT"));
  client.println("QUIT");
  if(!eRcv()) return 0;
 
  client.stop();
 
  Serial.println(F("disconnected"));
 
  return 1;
}
 
byte RRMail::eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }
 
  respCode = client.peek();
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  if(respCode >= '4')
  {
    efail();
    return 0;  
  }
 
  return 1;
}
 
 
void RRMail::efail()
{
  byte thisByte = 0;
  int loopCount = 0;
 
  client.println(F("QUIT"));
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  client.stop();
 
  Serial.println(F("disconnected"));
}
