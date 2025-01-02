#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "Adafruit_TCS34725.h"
#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include "actions.h"
#include "globals.h"
#include "data.h"

void setup () {
    // CONFIGURATION
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

    actions::registerSerialCommands();
    data::system_data_setup();

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

        // Write metadata and header into file.txt
        actions::sd_save_metadata();
        actions::save_header();
        // Send metadata and header through serial communication
        actions::print_serial_metadata();
        actions::print_serial_header();

        //Set up the actual measurement schedule

        globals::mainTimerId = globals::timer.setInterval(globals::period_ms, [](){
            // Save time
            actions::sd_save_date();
            actions::print_serial_date();
            
            // Measurement
            for (int i = 0; i < globals::measures; i++) {
                actions::measure_TCS34725();
                actions::sd_save_data();
                actions::print_serial_rgb_data();
            }
            actions::sd_save_new_line();
        });
    });
}

void loop () {
    // Read time
    globals::sC.readSerial(Serial);
    globals::timer.run();    
}
