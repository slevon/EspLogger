#ifndef RRTIME
#define RRTIME

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <Time.h>


class RRTime{
    public:
      RRTime();
      ~RRTime();
      static time_t getTime();
      //void enableAutoSync();  //Enables the autosync each 12hrs
      void digitalClockDisplay();
      String timeString();
      String dateString();
      String dateTimeString();

    private:
      String getDigits(int digits);
      void sendNTPpacket(IPAddress &address);
  };


#endif
