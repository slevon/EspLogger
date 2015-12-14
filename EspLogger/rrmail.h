#ifndef RRMAIL
#define RRMAIL


#include <ESP8266WiFi.h>

class RRMail{
  RRMail();
  ~RRMail();

void  sendMail(String to,String subject,String body);
  
};


#endif
