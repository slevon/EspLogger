
#ifndef WEMOSRELAY
#define WEMOSRELAY
#include <Arduino.h>
#include "main.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>


class WemosRelay{
   public:
    WemosRelay(){
      pinState=LOW;
      relayPin=RELAY_PIN;
      interval=1000;
      toggled=true;
      lastToggle=millis();
      relayChangeUrl[0]='\0';
      pinMode(relayPin, OUTPUT);
      digitalWrite(relayPin, LOW);
      };
    ~WemosRelay(){};
     void setUrl(const char * url){
      strcpy(relayChangeUrl,url);
     }
    
    byte state(){return pinState;};
   
    boolean popToggled(){
        if(!toggled){
            return false;
          }
         toggled=false;
         return true;
      }
      
    boolean toggle(){
         if(!isBlocked()){ //Do not toogle too fast
               // toggle the relay
             if(pinState == LOW){ 
              pinState = HIGH;
             }else{
              pinState = LOW;
             }
             digitalWrite(relayPin, pinState);
             lastToggle=millis();
             toggled=true;
             //Send state:
            // sendState();
             return true;
          }
          return false;
     }
    boolean set(){
        if(state() == LOW){
            return toggle();
          }
          return false;
        }
    boolean unset(){
        if(state() == HIGH){
            return toggle();
          }
          return false;
    }
   boolean setState(bool state){
        if(state){
          return set();
        }else{
            return unset();
        }
    }
    boolean isBlocked(){
      if(millis() - lastToggle >= interval){
        return false;
      }else{
        return true;
      }
    }

   void run(){

    DEBUGPRINT.println("TODO: check if relay button is pressed");
    
   }
    
  private:
    void sendState(){
          //insert the current value
          String url(relayChangeUrl);
          if(url.length()>3){  //only if set
            HTTPClient http;
            http.setTimeout(3000);
            url.replace("{state}",String(state()));
            DEBUGPRINT.println(String("Sending http state: ")+url);
            http.begin(String("http://")+url); //HTTP
            int httpCode = http.GET();
            
            if(httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                DEBUGPRINT.printf("[HTTP] GET... code: %d\n", httpCode);
    
                // file found at server
                if(httpCode == HTTP_CODE_OK) {
                    String payload = http.getString();
                    DEBUGPRINT.println(payload);
                }
            } else {
                DEBUGPRINT.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }
    
            http.end();
          }
      }

  private:
    byte pinState;
    boolean toggled;
    int relayPin;
    boolean pinInverted;
    unsigned long lastToggle;
    unsigned long interval;
    char relayChangeUrl[64];
  };

  #endif
