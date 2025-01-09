#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include "actions.h"
#include "globals.h"
#include "data.h"

#define IS_KDUPRO_DEBUG_ON 1

void setup () {
    delay(100);
    //LEDs
    pinMode(REDLED, OUTPUT);
    pinMode(BLUELED, OUTPUT);

    digitalWrite(REDLED, LED_OFF); //On indicates error state during initialization
    digitalWrite(BLUELED, LED_ON); //On indicates Initialization Period
    //All lights off while measurement session is running

    // Wifi (disable) 
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(100);

    // Start communication with Serial. Need Wire.begin.
    Wire.begin();
    delay(1000);

    Serial.begin(BAUDRATE);
    #ifdef IS_KDUPRO_DEBUG_ON
    unsigned long startTime = millis();
    while (!Serial && (millis() - startTime < 10000)) {
        yield();
    }
    delay(1500);
    Serial.println("Serial monitor is active");
    #endif

    actions::registerSerialCommands();
    actions::init_rtc();
    actions::init_sd();
    actions::init_tcs();
    data::system_data_setup();

    // Initial wait
    Serial.print("Waiting ");
    Serial.print(globals::initial_wait_s);
    Serial.println("s to start data collection");

    globals::timer.setTimeout(globals::initial_wait_s * 1000, [](){
        digitalWrite(BLUELED, LED_OFF);
        actions::start_data_recording_process();
    });
}

void loop () {
    // Read time
    globals::sC.readSerial(Serial);
    globals::timer.run();    
}
