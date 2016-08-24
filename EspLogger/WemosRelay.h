

class WemosRelay{
  
  WemosRelay(){
    state=LOW;
    relayPin=D1;
    interval=1000;
    lastToggle=millis();
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);
    };
  ~WemosRelay(){};
  byte state(){return state;};
  
  boolean toggle(){
       if(!isBlocked()){
             // toggle the relay
           if(relayState == LOW){ 
            relayState = HIGH;
           }else{
            relayState = LOW;
           }
           digitalWrite(relayPin, relayState);
           lastToggle=millis();
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
  

  private:
  byte pinState;
  int relayPin;
  unsigned long lastToggle;
  unsigned long interval;
  };
