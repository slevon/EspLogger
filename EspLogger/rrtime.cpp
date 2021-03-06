#include "rrtime.h"
  
  
unsigned long RRTime::lastSync =0;  //decalre

RRTime::RRTime(){
}

RRTime::~RRTime(){
}



//This gets the time from the server and sets the system Time.
//This function is also used as syncProvider:
time_t RRTime::getTime(){
   unsigned int localPort = 8888;      // local port to listen for UDP packets
  
  /* Don't hardwire the IP address or we won't get the benefits of the pool.
   *  Lookup the IP address for the host name instead */
  //IPAddress timeServerIP(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
  IPAddress timeServerIP; // time.nist.gov NTP server address
  const char* ntpServerName = "europe.pool.ntp.org";
  
  static const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
  
  byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packet
  const int timeZone = 1;     // Central European Time
  WiFiUDP udp; // A UDP instance to let us send and receive packets over UDP
  udp.begin(localPort);
  DEBUGPRINT.print("Local port: ");
  DEBUGPRINT.println(udp.localPort());
  DEBUGPRINT.println("waiting for sync");

     //get a random server from the pool
  if(!WiFi.hostByName(ntpServerName, timeServerIP)){
    DEBUGPRINT.print("ERROR; Could not resolve IP using ");
    IPAddress fallBack(132, 163, 4, 102);
    timeServerIP = fallBack;
    DEBUGPRINT.println(timeServerIP);
    } 
  DEBUGPRINT.print("Timesever IP:");
  DEBUGPRINT.println(timeServerIP);
  while (udp.parsePacket() > 0) ; // discard any previously received packets
  DEBUGPRINT.println("Transmit NTP Request");
  // set all bytes in the buffer to 0
  /////////////////////////////////////////////////////////////////////////
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  udp.beginPacket(timeServerIP, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
  /////////////////////////////////////////////////////////////////////////
  uint32_t beginWait = millis();
  while (millis() - beginWait < 3000) {
    int size = udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      DEBUGPRINT.println("Receive NTP Response");
      udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      //store last sync:
      lastSync = millis();
      
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  DEBUGPRINT.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}


void RRTime::digitalClockDisplay(){
  // digital clock display of the time
  DEBUGPRINT.print(hour());
  DEBUGPRINT.print(":");
  DEBUGPRINT.print(getDigits(minute()));
  DEBUGPRINT.print(":");
  DEBUGPRINT.print(getDigits(second()));
  DEBUGPRINT.print(" ");
  DEBUGPRINT.print(day());
  DEBUGPRINT.print(".");
  DEBUGPRINT.print(month());
  DEBUGPRINT.print(".");
  DEBUGPRINT.print(year()); 
  DEBUGPRINT.println(); 
}

String RRTime::timeString(){
  return getDigits(hour())+":"+getDigits(minute())+":"+getDigits(second());
}
String RRTime::dateString(){
   return getDigits(day())+"."+getDigits(month())+"."+year();
}
String RRTime::dateTimeString(){
  return timeString()+" "+dateString();
}
unsigned long  RRTime::getLastSync(){
  return lastSync;
}
String RRTime::getStatus(){
  if(timeStatus() == timeNotSet){
    return F("Not set");
  }
  if(timeStatus() == timeNeedsSync){
    return F("Needs sync");
  }
  if(timeStatus() == timeNotSet){
    return F("Ok");
  }
  
}


String RRTime::getDigits(int digits){
  String ret;
  if(digits < 10)
    ret+="0";
  return ret+digits;
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
 
}


