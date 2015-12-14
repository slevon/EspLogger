#ifndef RRMAIL
#define RRMAIL


#include <ESP8266WiFi.h>

class RRMail{
  public:
    RRMail();
    ~RRMail();
  
    byte sendMail(String to,String subject,String body);
  private:
    WiFiClient client;
    byte eRcv();
    void efail();
  
};


#endif
