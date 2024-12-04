#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "Adafruit_TCS34725.h"
#include <ESP8266WiFi.h>
#include "sCommand.h"
#include <SimpleTimer.h>

sCommand sC;
#define SERIALCOMMAND_DEBUG

// Settings 
int real_time_clock_setup = 120;     // Time to wait to set up time of RTC (in seconds)
int initial_wait_s = 60;               // Time to wait before start the loop (in seconds)

int measures = 60;                   // Number of measurements to do[1, 59]
int period_ms = 60000;                      // Sampling period (ms)
int sample_counter = 1;              // Counter of measurements
int mainTimerId = 0;

float depth = 0.5;                   // Absolute depth of the device [0.1, 30] (in meters)

// Metadata
String name = "Kdupro01";                   // Name of the module
String buoy = "1";                          // Name of the buoy
String country = "Colombia";                // Name of the country
String place = "Akuara";                    // Text with place of deployment
String maker = "UDEA";                      // Maker name
String curator = "ICM-CSIC";                // Curator name
String email = "rodero@icm.csic.es";        // Email of the curator
String sensors = "TCS34725";                // List with name of used sensors "Sensor 1, ..., Sensor n"
String description = "calibration";                    // Description (optional)
String units = "counts, counts, counts, counts";    // Units of the measurements "Unit 1, ..., Unit n"

// MONOCLE Metadata
String latitude = "41.383189";              // Latitude
String longitude = "2.197949";              // Longitude
String altitude = "0";                      // Altitude  
String ref_coord_system = "WGS84";          // Reference Coordinate System
String location_source = "GNSS";            // Source of the Geodesic information
String time_source = "internet time pool";  // Source of the Time information
String processing_level = "0";              // Defined by manufacturer and described in the reference documentation.
String processing_procedure = "https://git.csic.es/kduino/kdupro";           // Reference to protocols and algorithms describing the steps involved in data processing
String processing_version = "build";        // Version of the data processing software
String processing_revision = "0";           // Incremental version of the processed data
String calibration_procedure = "https://git.csic.es/kduino/kdupro";          // For calibrated data: documentation describing the calibration procedure. Can be the same as Processing procedure reference
String calibration_reference = "0";         // Identifier of calibration information
String calibration_time = "0";              // Date/time stamp of applicable (uncalibrated data, if available) or applied (calibrated data) sensor calibration.
String calibration_version = "0";           // Version of the calibration processing software
// sensor_id, platform_id, deployment_id, sample_id generated in method "generate_metadata_id"
String sensor_id = "";              // Unique identifiers used to prevent data duplication with data consumers
String platform_id = "";            // Platform serial number or randomly assigned identifier (UUID) used with all connected sensors. May be left empty if not applicable.
String deployment_id = "";          // Randomly assigned identifier (UUID) specific to deployment sequence (e.g. cruise, campaign, vertical profile) of a specific sensor. Not shared with other sensors.
String sample_id = "";              // Randomly assigned identifier (UUID) generated with each distinct data record from any set of sensors belonging to a single observation.
String observer_id = "";            // Randomly assigned identifier (UUID) repeated with each data record from this and/or other sensors when operated by a specific observer.
String owner_contact = "jpiera@icm.csic.es";         // An email address where the owner of the data can be contacted now and in future
String operator_contact = "rodero@icm.csic.es";      // An email address where the current operator can be contacted
String license = "MIT License";                      // A licence string or coding that is either self-explanatory or detailed in the License_reference field.
String license_reference = "https://opensource.org/licenses/MIT";           // A reference describing the data license in detail.
String embargo_date = "";                   // A date following which the data may be used according to the specified license. Used, for example, to hide the data record in NRT visualization until quality control is completed.
// Time set up in the serial monitor. Format YYYY-MM-DDThh:mm:ssZ  // Character string formatted according to ISO8601
String datetime = "YYYY-MM-DDThh:mm:ssZ";

// Contants
#define BAUDRATE 9600
#define REDLED 0
#define BLUELED 2
#define BATPIN A0 // Power management

// Vars
RTC_PCF8523 rtc;
const int chipSelect_SD = 15;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);
uint16_t r, g, b, c; 
float battery_level;
DateTime now;
String filename;
SimpleTimer timer(Serial); //Providing the Serial object is only necessary to see debug output

// Function declaration
void measure_TCS34725();
void print_serial_rgb_data();
void print_serial_metadata();
void print_serial_header();
void print_serial_date();
void sd_save_data();
void sd_save_new_line();
void sd_save_date();
void sd_save_metadata();
void save_header();
void update_rtc_serial(sCommand& sC, Stream& S);
void get_rtc_serial(sCommand& sC, Stream& S);

void generate_metadata_id();
void generate_filename();
String get_datetime();

struct Commands {
    const char *SET_RTC = "T";
    const char *GET_RTC = "GT";
};
Commands commands;

void setup () {
    // CONFIGURATION
    delay(100);
    
    // Red Led
    pinMode(REDLED, OUTPUT);
    digitalWrite(REDLED, HIGH);
    pinMode(BLUELED, OUTPUT);
    digitalWrite(BLUELED, LOW);

    // Wifi (disable) 
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(100);

    // Start communication with Serial. Need Wire.begin.
    //*************************************************
    // If Serial does not work properly --> increment delay(1000, 2000, 3000...)
    //*************************************************
    Wire.begin();
    delay(1000);

    // Serial
    Serial.begin(BAUDRATE);

    sC.addCommand(commands.SET_RTC, update_rtc_serial);
    sC.addCommand(commands.GET_RTC, get_rtc_serial);

    delay(100);
    Serial.println("");
    // RTC
    Serial.print("Initializing RTC.");
    if (! rtc.begin()) {
        digitalWrite(REDLED, LOW);
        while (1){
            Serial.println("Couldn't find RTC");
            delay(10000);
        }
    }
    Serial.println("RTC Init Done.");
    
    // SD
    Serial.print("Initializing SD card.");
    pinMode(SS, OUTPUT);
    if (!SD.begin(chipSelect_SD)) {
        digitalWrite(REDLED, LOW);
        while (1){
            Serial.println("Card failed, or not present");
            delay(10000);
        }
    }
    Serial.println("SD Init Done.");
    
    // TCS34725
    Serial.print("Initializing TCS34725.");
    while (!tcs.begin()) {
        digitalWrite(REDLED, LOW);
        Serial.println("No TCS34725 found");
        delay(10000);
    }
    digitalWrite(REDLED, HIGH);
    tcs.clearInterrupt();
    Serial.println("TCS34725 Init Done.");
    
    
    // Initial wait
    Serial.print("Waiting ");
    Serial.print(initial_wait_s);
    Serial.println(" seconds");

    timer.setTimeout(initial_wait_s * 1000, [](){
        digitalWrite(BLUELED, HIGH);


        generate_metadata_id();
        generate_filename();
        // Write metadata and header into file.txt
        sd_save_metadata();
        save_header();
        // Send metadata and header through serial communication
        print_serial_metadata();
        print_serial_header();

        digitalWrite(BLUELED, HIGH);

        //Set up the actual measurement schedule

        mainTimerId = timer.setInterval(period_ms, [](){
            // Save time
            sd_save_date();
            print_serial_date();
            
            // Measurement
            for (int i = 0; i < measures; i++) {
                measure_TCS34725();
                sd_save_data();
                print_serial_rgb_data();
            }
            sd_save_new_line();
        });
    });
}

void loop () {
    // Read time
    sC.readSerial(Serial);
    timer.run();    
}

////////////////////////////////////////////////////////////
////// GENERATE METADATA ID AND FILE NAME //////////////////
////////////////////////////////////////////////////////////

void generate_metadata_id(){
    /* update the ids from metadata */

    String date = get_datetime().substring(0, 10);
    String time_sample = get_datetime().substring(11, 16);
    String rnd_number = String(random(0xffff), HEX);

    sensor_id = WiFi.macAddress();
    sensor_id.replace(":", "");
    platform_id = buoy + "_" + country + "_" + place;
    deployment_id = sensor_id + "_" + name + "_" + depth + "_" + rnd_number;
    sample_id = platform_id + "_" + time_sample;
    observer_id = operator_contact + "_" + date + "_" + time_sample;
    
}

void generate_filename(){
    /* generate name of the file */
    String date = get_datetime().substring(0, 10);
    filename = date + "_" + "buoy" +  platform_id + "_" + depth + ".txt";
}

////////////////////////////////////////////////////////////
//////////////////// MEASUREMENTS //////////////////////////
////////////////////////////////////////////////////////////

void measure_TCS34725(){
        
    uint16_t r_, g_, b_, c_;
    // Measure
    tcs.getRawData(&r_, &g_, &b_, &c_);
    
    // Save data into global variables
    r = r_;
    g = g_;
    b = b_;
    c = c_;
}

////////////////////////////////////////////////////////////
//////////////////// SERIAL COMMUNICATION //////////////////
////////////////////////////////////////////////////////////

void print_serial_date(){
    /*It sends the data throught the serial  communication*/
    Serial.println(get_datetime());
}

void print_serial_rgb_data(){
    /*It sends the data throught the serial  communication*/
    Serial.print(" ");
    Serial.print(r, DEC);
    Serial.print(" ");
    Serial.print(g, DEC);
    Serial.print(" ");
    Serial.print(b, DEC);
    Serial.print(" ");
    Serial.print(c, DEC);
    Serial.println("");
}

void print_serial_metadata(){
    /*It sends the metadata info throught the serial  communication*/
    Serial.println("METADATA");
    Serial.print("initial_wait_s: ");
    Serial.println(initial_wait_s, DEC);
    Serial.print("measures: ");
    Serial.println(measures, DEC);
    Serial.print("period_ms: ");
    Serial.println(period_ms, DEC);
    Serial.print("depth: ");
    Serial.println(depth);
    Serial.print("sample_counter: ");
    Serial.println(sample_counter);
    
    Serial.print("name: ");
    Serial.println(name);
    Serial.print("buoy: ");
    Serial.println(buoy);
    Serial.print("place: ");
    Serial.println(place);
    Serial.print("country: ");
    Serial.println(country);
    Serial.print("maker: ");
    Serial.println(maker);
    Serial.print("curator: ");
    Serial.println(curator);
    Serial.print("email: ");
    Serial.println(email);
    Serial.print("sensors: ");
    Serial.println(sensors);
    Serial.print("description: ");
    Serial.println(description);
    Serial.print("units: ");
    Serial.println(units);

    Serial.print("latitude: ");
    Serial.println(latitude);
    Serial.print("longitude: ");
    Serial.println(longitude);
    Serial.print("altitude: ");
    Serial.println(altitude);
    Serial.print("ref_coord_system: ");
    Serial.println(ref_coord_system);
    Serial.print("location_source: ");
    Serial.println(location_source);
    Serial.print("time_source: ");
    Serial.println(time_source);
    Serial.print("processing_level: ");
    Serial.println(processing_level);
    Serial.print("processing_procedure: ");
    Serial.println(processing_procedure);
    Serial.print("processing_version: ");
    Serial.println(processing_version);
    Serial.print("processing_revision: ");
    Serial.println(processing_revision);
    Serial.print("calibration_procedure: ");
    Serial.println(calibration_procedure);
    Serial.print("calibration_reference: ");
    Serial.println(calibration_reference);
    Serial.print("calibration_time: ");
    Serial.println(calibration_time);
    Serial.print("calibration_version: ");
    Serial.println(calibration_version);
    Serial.print("sensor_id: ");
    Serial.println(sensor_id);
    Serial.print("platform_id: ");
    Serial.println(platform_id);
    Serial.print("deployment_id: ");
    Serial.println(deployment_id);
    Serial.print("sample_id: ");
    Serial.println(sample_id);
    Serial.print("observer_id: ");
    Serial.println(observer_id);
    Serial.print("owner_contact: ");
    Serial.println(owner_contact);
    Serial.print("operator_contact: ");
    Serial.println(operator_contact);
    Serial.print("license: ");
    Serial.println(license);
    Serial.print("license_reference: ");
    Serial.println(license_reference);
    Serial.print("embargo_date: ");
    Serial.println(embargo_date);

    Serial.print("time: ");
    Serial.println(get_datetime());
}

void print_serial_header(){
    /*It sends the header info of the data through the serial
    communication*/
    Serial.println("DATA");
    Serial.println("DATE HOUR RED GREEN BLUE CLEAR");
}

////////////////////////////////////////////////////////////
//////////////////// SD MANAGEMENT /////////////////////////
////////////////////////////////////////////////////////////

File sd_open_file() {
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File data_file = SD.open(filename, FILE_WRITE);

    while (! data_file) {
        digitalWrite(REDLED, LOW);
        // Wait forever since we cant write data
        Serial.println("error opening " + filename);
        delay(10000);
        data_file = SD.open(filename, FILE_WRITE);
    }
    digitalWrite(REDLED, HIGH);

    return data_file;    
}

void sd_save_data(){
    File data_file = sd_open_file();
    // Save data
    data_file.print(" ");
    data_file.print(r, DEC);
    data_file.print(" ");
    data_file.print(g, DEC);
    data_file.print(" ");
    data_file.print(b, DEC);
    data_file.print(" ");
    data_file.print(c, DEC); 
    data_file.flush();
    data_file.close();
}

void sd_save_date(){
    File data_file = sd_open_file();
 
    // Save data
    data_file.print(get_datetime()); 
    data_file.flush();

    // Close dataFile
    data_file.close();
}

void sd_save_new_line(){
    File data_file = sd_open_file();
 
    // Save new line
    data_file.println(""); 
    data_file.flush();
    data_file.close();
}

void sd_save_metadata(){
    File data_file = sd_open_file();
 
    // Save metadata
    data_file.println(""); data_file.flush();
    data_file.println("METADATA"); data_file.flush();
    data_file.print("initial_wait_s: "); data_file.flush();
    data_file.println(initial_wait_s, DEC); data_file.flush();
    data_file.print("measures: "); data_file.flush();
    data_file.println(measures, DEC); data_file.flush();
    data_file.print("period_ms: "); data_file.flush();
    data_file.println(period_ms, DEC); data_file.flush();
    data_file.print("depth: "); data_file.flush();
    data_file.println(depth); data_file.flush();
    data_file.print("sample_counter: "); data_file.flush();
    data_file.println(sample_counter); data_file.flush();

    data_file.print("name: "); data_file.flush();
    data_file.println(name); data_file.flush();
    data_file.print("buoy: "); data_file.flush();
    data_file.println(buoy); data_file.flush();
    data_file.print("place: "); data_file.flush();
    data_file.println(place); data_file.flush();
    data_file.print("country: "); data_file.flush();
    data_file.println(country); data_file.flush();
    data_file.print("maker: "); data_file.flush();
    data_file.println(maker); data_file.flush();
    data_file.print("curator: "); data_file.flush();
    data_file.println(curator); data_file.flush();
    data_file.print("email: "); data_file.flush();
    data_file.println(email); data_file.flush();
    data_file.print("sensors: "); data_file.flush();
    data_file.println(sensors); data_file.flush();
    data_file.print("description: "); data_file.flush();
    data_file.println(description); data_file.flush();
    data_file.print("units: "); data_file.flush();
    data_file.println(units); data_file.flush();

    data_file.print("latitude: "); data_file.flush();
    data_file.println(latitude); data_file.flush();
    data_file.print("longitude: "); data_file.flush();
    data_file.println(longitude); data_file.flush();
    data_file.print("altitude: "); data_file.flush();
    data_file.println(altitude); data_file.flush();
    data_file.print("ref_coord_system: "); data_file.flush();
    data_file.println(ref_coord_system); data_file.flush();
    data_file.print("location_source: "); data_file.flush();
    data_file.println(location_source); data_file.flush();
    data_file.print("time_source: "); data_file.flush();
    data_file.println(time_source); data_file.flush();
    data_file.print("processing_level: "); data_file.flush();
    data_file.println(processing_level); data_file.flush();
    data_file.print("processing_procedure: "); data_file.flush();
    data_file.println(processing_procedure); data_file.flush();
    data_file.print("processing_version: "); data_file.flush();
    data_file.println(processing_version); data_file.flush();
    data_file.print("processing_revision: "); data_file.flush();
    data_file.println(processing_revision); data_file.flush();
    data_file.print("calibration_procedure: "); data_file.flush();
    data_file.println(calibration_procedure); data_file.flush();
    data_file.print("calibration_reference: "); data_file.flush();
    data_file.println(calibration_reference); data_file.flush();
    data_file.print("calibration_time: "); data_file.flush();
    data_file.println(calibration_time); data_file.flush();
    data_file.print("calibration_version: "); data_file.flush();
    data_file.println(calibration_version); data_file.flush();
    data_file.print("sensor_id: "); data_file.flush();
    data_file.println(sensor_id); data_file.flush();
    data_file.print("platform_id: "); data_file.flush();
    data_file.println(platform_id); data_file.flush();
    data_file.print("deployment_id: "); data_file.flush();
    data_file.println(deployment_id); data_file.flush();
    data_file.print("sample_id: "); data_file.flush();
    data_file.println(sample_id); data_file.flush();
    data_file.print("observer_id: "); data_file.flush();
    data_file.println(observer_id); data_file.flush();
    data_file.print("owner_contact: "); data_file.flush();
    data_file.println(owner_contact); data_file.flush();
    data_file.print("operator_contact: "); data_file.flush();
    data_file.println(operator_contact); data_file.flush();
    data_file.print("license: "); data_file.flush();
    data_file.println(license); data_file.flush();
    data_file.print("license_reference: "); data_file.flush();
    data_file.println(license_reference); data_file.flush();
    data_file.print("embargo_date: "); data_file.flush();
    data_file.println(embargo_date); data_file.flush();

    data_file.print("time: "); data_file.flush();
    data_file.println(get_datetime()); data_file.flush();

    // Close dataFile
    data_file.close();
}

void save_header(){
    File data_file = sd_open_file();
 
    // Save header
    data_file.println("DATA"); data_file.flush();
    data_file.println("TIME RED GREEN BLUE CLEAR");
    data_file.flush();
    // Close dataFile
    data_file.close();
}


///////////////////////////////////////////
///////////// ACTIONS /////////////////////
///////////////////////////////////////////


void get_rtc_serial(sCommand& sC, Stream& S) {
    print_serial_date();
}

void update_rtc_serial(sCommand& sC, Stream& S) {

    if ( sC.next() == NULL) {
        S.println("Missing new RTC time value (YYYYMMDDHHMMSS)");
    } else {
        const char* dateTimeStr = sC.current();
        S.printf("Received: %s\n", dateTimeStr);
        int year, month, day, hour, minute, second;

        // Parse the date-time string
        if (sscanf(dateTimeStr, "%4d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &minute, &second) == 6) {
            // Successfully parsed all components
            S.printf("Parsed date-time: %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);

            // Now you can use the parsed values to update the RTC
            // Example: rtc.adjust(DateTime(year, month, day, hour, minute, second));

            rtc.adjust(DateTime(year, month, day, hour, minute, second));
            // Read time
            Serial.println("Real Time Clock Updated");
            print_serial_date();

        } else {
            S.println("Invalid date-time format. Expected YYYYMMDDHHMMSS.");
        }
    }
}

String get_datetime() {
    /* Updates datetime string with correct format */

    now = rtc.now();
    char buffer [31] = "";
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d:%03dZ", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), millis() % 1000);
    datetime = String(buffer);

    return datetime;
}
