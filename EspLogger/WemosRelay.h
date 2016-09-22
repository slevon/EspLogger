
#ifndef WEMOSRELAY
#define WEMOSRELAY
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>


class WemosRelay{
  public:
    WemosRelay(){
      pinState=LOW;
      relayPin=D1;
      interval=1000;
      lastToggle=millis();
      relayChangeUrl[0]='\0';
      pinMode(relayPin, OUTPUT);
      digitalWrite(relayPin, LOW);
      };
    ~WemosRelay(){};
    void setUrl(char * url){
      snprintf( relayChangeUrl,sizeof(relayChangeUrl),"%s",url);
     }
    
    byte state(){return pinState;};
    
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
             //Send state:
             sendState();
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
    boolean isBlocked(){
      if(millis() - lastToggle >= interval){
        return false;
      }else{
        return true;
      }
    }

   void run(){

    Serial.println("TODO: check if relay button is pressed");
    
   }
    
  private:
    void sendState(){
      
          HTTPClient http;
  
          //insert the current value
          String url(relayChangeUrl);
          if(url.length()<1){  //only if set
            return;
          }
          url.replace("{state}",String(state()));
          Serial.println(String("Sending http state: ")+url);
          http.begin(String("http://")+url); //HTTP
          int httpCode = http.GET();
          
          if(httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  
              // file found at server
              if(httpCode == HTTP_CODE_OK) {
                  String payload = http.getString();
                  Serial.println(payload);
              }
          } else {
              Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          }
  
          http.end();
        
      
      }

  private:
    byte pinState;
    int relayPin;
    boolean pinInverted;
    unsigned long lastToggle;
    unsigned long interval;
    char relayChangeUrl[64];
  };

  #endif
