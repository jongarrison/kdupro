#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <SD.h>
#include <Arduino.h>
#include "sCommand.h"
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include "Adafruit_TCS34725.h"

#define SERIALCOMMAND_DEBUG

// Contants
#define BAUDRATE 9600
#define REDLED 0
#define BLUELED 2
#define BATPIN A0 // Power management
#define LED_ON LOW
#define LED_OFF HIGH
#define INIT_FAILURE_DELAY 1000

namespace globals {

    extern int mainTimerId; // Timer ID for the main loop

    // Settings 
    extern int initial_wait_s;               // Time to wait before start the loop (in seconds)

    extern int measures;                   // Number of measurements to do[1, 59]
    extern int period_ms;                      // Sampling period (ms)
    extern int sample_counter;              // Counter of measurements

    extern float depth;                   // Absolute depth of the device [0.1, 30] (in meters)

    // Vars
    extern sCommand sC;
    extern RTC_PCF8523 rtc;
    extern const int chipSelect_SD;
    extern Adafruit_TCS34725 tcs;
    extern uint16_t r, g, b, c; 
    extern float battery_level;
    extern DateTime now;
    extern String filename;
    extern SimpleTimer timer; //Providing the Serial object is only necessary to see debug output


    struct Commands {
        const char *SET_RTC = "T";
        const char *GET_RTC = "GT";
        const char *LS_SD = "LS";
        const char *CAT_SD = "CAT";
    };
    extern Commands commands; 

}

#endif /* GLOBALS_H_ */