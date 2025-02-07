#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <SD.h>
#include <Arduino.h>
#include "sCommand.h"
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
//#include "Adafruit_TCS34725.h"
#include <Adafruit_AS7341.h>
#include "SimpleDictionary.h"
#include <array>

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
    extern SimpleTimer timer;       
    extern bool isDataCollectionPaused;
    extern const String sd_settings_file;

    // Settings 
    extern int initial_wait_s;      // Time to wait before start the loop (in seconds)
    extern int measures;            // Number of measurements to do[1, 59]
    extern int period_ms;           // Sampling period (ms)
    extern int sample_counter;      // Counter of measurements

    // Derived Metadata
    extern String depth;            // Absolute depth of the device [0.1, 30] (in meters)
    extern String sensor_id;        // Unique identifiers used to prevent data duplication with data consumers
    extern String platform_id;      // Platform serial number or randomly assigned identifier (UUID) used with all connected sensors. May be left empty if not applicable.
    extern String deployment_id;    // Randomly assigned identifier (UUID) specific to deployment sequence (e.g. cruise, campaign, vertical profile) of a specific sensor. Not shared with other sensors.
    extern String sample_id;        // Randomly assigned identifier (UUID) generated with each distinct data record from any set of sensors belonging to a single observation.
    extern String observer_id;      // Randomly assigned identifier (UUID) repeated with each data record from this and/or other sensors when operated by a specific observer.

    extern sCommand sC;
    extern const int chipSelect_SD;
    //extern Adafruit_TCS34725 tcs;   //Color sensor
    extern Adafruit_AS7341 as7341; //Spectral sensor

    // extern uint16_t r, g, b, c; 
    extern uint16_t lightReadings[12];
    extern String filename;

    struct metaDataDictConfig {
        static const unsigned int keySize = 40;
        static const unsigned int valueSize = 40;
        static const unsigned int elementCount = 40;
    };

    extern SimpleDictionary<std::array<char, metaDataDictConfig::valueSize>, metaDataDictConfig::keySize, metaDataDictConfig::elementCount> metadata;

    extern RTC_PCF8523 rtc;
    

}

#endif /* GLOBALS_H_ */