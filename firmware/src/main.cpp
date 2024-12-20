#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "Adafruit_TCS34725.h"
#include <ESP8266WiFi.h>
#include "sCommand.h"
#include <SimpleTimer.h>
#include "actions.h"
#include "globals.h"

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

    // Setup Serial API
    globals::sC.addCommand(globals::commands.LS_SD, actions::list_sd_files);
    globals::sC.addCommand(globals::commands.CAT_SD, actions::list_sd_file_contents);
    globals::sC.addCommand(globals::commands.SET_RTC, actions::update_rtc_serial);
    globals::sC.addCommand(globals::commands.GET_RTC, actions::get_rtc_serial);

    // RTC
    Serial.print("Initializing RTC.");
    if (!globals::rtc.begin()) {
        digitalWrite(REDLED, LED_ON);
        Serial.println("Couldn't find RTC");
        delay(INIT_FAILURE_DELAY);
    }
    digitalWrite(REDLED, LED_OFF);
    Serial.println("RTC Init Done.");
    
    // SD
    Serial.print("Initializing SD card.");
    pinMode(SS, OUTPUT);
    if (!SD.begin(globals::chipSelect_SD)) {
        digitalWrite(REDLED, LED_ON);
        Serial.println("Card failed, or not present");
        delay(INIT_FAILURE_DELAY);
    }
    digitalWrite(REDLED, LED_OFF);
    Serial.println("SD Init Done.");
    
    // TCS34725
    Serial.print("Initializing TCS34725.");
    while (!globals::tcs.begin()) {
        digitalWrite(REDLED, LED_ON);
        Serial.println("No TCS34725 found");
        delay(INIT_FAILURE_DELAY);
    }
    digitalWrite(REDLED, LED_OFF);
    globals::tcs.clearInterrupt();
    Serial.println("TCS34725 Init Done.");
    
    // Initial wait
    Serial.print("Waiting ");
    Serial.print(globals::initial_wait_s);
    Serial.println(" seconds");

    globals::timer.setTimeout(globals::initial_wait_s * 1000, [](){
        digitalWrite(BLUELED, LED_OFF);

        actions::generate_metadata_id();
        actions::generate_filename();
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
