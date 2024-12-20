#include "globals.h"

namespace globals {
    sCommand sC;
    int mainTimerId = 0; // Timer ID for the main loop

    // Settings 
    int initial_wait_s = 60;               // Time to wait before start the loop (in seconds)

    int measures = 60;                   // Number of measurements to do[1, 59]
    int period_ms = 60000;                      // Sampling period (ms)
    int sample_counter = 1;              // Counter of measurements

    float depth = 0.5;                   // Absolute depth of the device [0.1, 30] (in meters)

    // Vars
    RTC_PCF8523 rtc;
    const int chipSelect_SD = 15;
    Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);
    uint16_t r, g, b, c; 
    float battery_level;
    DateTime now;
    String filename;
    SimpleTimer timer(Serial); //Providing the Serial object is only necessary to see debug output

    Commands commands;    
}
