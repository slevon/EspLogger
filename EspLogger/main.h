

#ifndef MAIN_H
#define MAIN_H


#define VERSION "0.1"

#define MODULE WEMOS    // Allowed Values are: WEMOS, SONOFF
#define DHT_ACTIVE 1    // 1= has DHT 0 = does not have a DHT

#define DEBUGPRINT Serial
//#define DEBUGPRINT terminal
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

#if  MODULE == WEMOS
  #define APP_NAME        "Wemos Module"
  #define LED_PIN         BUILTIN_LED
  #define LED_INVERTED    1  //(0 -> 1=on 0=off   |   1 -> 1=off 0=on)
  #define RELAY_PIN       5
  #define BUTTON_PIN      4
  #define DHT_PIN         2 //No PIN 7
  #define DHT_TYPE        DHT22
  #define PULSE_PIN       14

#elif MODULE == SONOFF
  #define APP_NAME        "SONOFF Module"
  #define LED_PIN         13
  #define LED_INVERTED    1  //(0 -> 1=on 0=off   |   1 -> 1=off 0=on)
  #define RELAY_PIN       12
  #define BUTTON_PIN      0
  #define DHT_PIN         4
  #define DHT_TYPE        DHT22
  #define PULSE_PIN       14  //TODO
  
#else
  #error "Define either WEMOS oder SONOFF as Mudule-Name"

#endif



#endif
