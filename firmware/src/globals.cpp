#include "globals.h"

namespace globals {
    sCommand sC;
    int mainTimerId = 0; // Timer ID for the main loop
    SimpleTimer timer(Serial); //Providing the Serial object is only necessary to see debug output
    bool isDataCollectionPaused = false;

    const String sd_settings_file = "settings.txt";

    // Settings 
    int initial_wait_s = 60;               // Time to wait before start the loop (in seconds)
    int measures = 60;                   // Number of measurements to do[1, 59]
    int period_ms = 60000;                      // Sampling period (ms)
    // int sample_counter = 1;              // Counter of measurements

    // Derived Metadata
    String sensor_id = "";
    String platform_id = "";
    String deployment_id = "";
    String sample_id = "";
    String observer_id = "";

    // Vars
    RTC_PCF8523 rtc;

    const int chipSelect_SD = 15;
    //Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);
    Adafruit_AS7341 as7341 = Adafruit_AS7341();

    // uint16_t r, g, b, c; 
    uint16_t lightReadings[12];
    String filename;
 
    SimpleDictionary<std::array<char, metaDataDictConfig::valueSize>, metaDataDictConfig::keySize, metaDataDictConfig::elementCount> metadata;    
}
