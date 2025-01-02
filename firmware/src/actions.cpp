#include "actions.h"

namespace actions {

    Commands commands;


    void registerSerialCommands() {
        globals::sC.addCommand(commands.LS_SD, list_sd_files);
        globals::sC.addCommand(commands.CAT_SD, list_sd_file_contents);
        globals::sC.addCommand(commands.SET_RTC, update_rtc_serial);
        globals::sC.addCommand(commands.GET_RTC, get_rtc_serial);
     
    }

    ////////////////////////////////////////////////////////////
    //////////////////// MEASUREMENTS //////////////////////////
    ////////////////////////////////////////////////////////////

    void measure_TCS34725(){
            
        uint16_t r_, g_, b_, c_;
        // Measure
        globals::tcs.getRawData(&r_, &g_, &b_, &c_);
        
        // Save data into global variables
        globals::r = r_;
        globals::g = g_;
        globals::b = b_;
        globals::c = c_;
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
        Serial.print(globals::r, DEC);
        Serial.print(" ");
        Serial.print(globals::g, DEC);
        Serial.print(" ");
        Serial.print(globals::b, DEC);
        Serial.print(" ");
        Serial.print(globals::c, DEC);
        Serial.println("");
    }

    void print_serial_metadata() {
        print_metadata_to_stream(Serial);
    }

    void print_metadata_to_stream(Stream& S){
        /*It sends the metadata info throught the serial  communication*/
        S.println("METADATA");
        S.print("initial_wait_s: ");
        S.println(globals::initial_wait_s, DEC);
        S.print("measures: ");
        S.println(globals::measures, DEC);
        S.print("period_ms: ");
        S.println(globals::period_ms, DEC);
        
        //Iterate over metadata keys
        int metadataSize = globals::metadata.size();
        for (int i = 0; i < metadataSize; i++) {
            S.print(String(globals::metadata.getKey(i)));
            S.print(": ");
            S.println(String(globals::metadata.getValue(i)));
        }

        S.print("time: ");
        S.println(get_datetime());
        S.flush();
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
        File data_file = SD.open(globals::filename, FILE_WRITE);

        while (! data_file) {
            digitalWrite(REDLED, LOW);
            // Wait forever since we cant write data
            Serial.println("error opening " + globals::filename);
            delay(10000);
            data_file = SD.open(globals::filename, FILE_WRITE);
        }
        digitalWrite(REDLED, HIGH);

        return data_file;    
    }

    void sd_save_data(){
        File data_file = sd_open_file();
        // Save data
        data_file.print(" ");
        data_file.print(globals::r, DEC);
        data_file.print(" ");
        data_file.print(globals::g, DEC);
        data_file.print(" ");
        data_file.print(globals::b, DEC);
        data_file.print(" ");
        data_file.print(globals::c, DEC); 
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
        actions::print_metadata_to_stream(data_file);
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

    void list_sd_files(sCommand& sC, Stream& S){
        File root = SD.open("/");
        while (true) {
            File entry = root.openNextFile();
            if (!entry) {
                // no more files
                break;
            }
            Serial.println(entry.name());
            entry.close();
        }
    }

    void list_sd_file_contents(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            S.println("Missing file name");
        } else {
            const char* filename = sC.current();
            File data_file = SD.open(filename);
            if (data_file) {
                while (data_file.available()) {
                    S.write(data_file.read());
                }
                data_file.close();
            } else {
                S.println("Error opening file");
            }
        }
    }

    void list_sd_file_size(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            S.println("Missing file name");
        } else {
            const char* filename = sC.current();
            File data_file = SD.open(filename);
            if (data_file) {
                S.println(data_file.size());
                data_file.close();
            } else {
                S.println("Error opening file");
            }
        }
    }

    void rm_sd_file(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            S.println("Missing file name");
        } else {
            const char* filename = sC.current();
            if (SD.remove(filename)) {
                S.println("File removed");
            } else {
                S.println("Error removing file");
            }
        }
    }

    void set_variable(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            S.println("Missing variable name");
        } else {
            const char* variable = sC.current();
            if ( sC.next() == NULL) {
                S.println("Missing variable value");
            } else {
                const char* value = sC.current();
                globals::metadata[variable] = value;
                S.println("Variable set");
            }
        }
    }

    void set_metadata(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            S.println("Missing metadata name");
        } else {
            const char* variableName = sC.current();
            if ( sC.next() == NULL) {
                S.println("Missing variable value");
            } else {
                strcpy(globals::metadata[variableName], sC.current());
                S.println("Variable set");
            }
        }
    }

    void init_sd() {
        Serial.print("Initializing SD card.");
        pinMode(SS, OUTPUT);
        if (!SD.begin(globals::chipSelect_SD)) {
            digitalWrite(REDLED, LED_ON);
            Serial.println("Card failed, or not present");
            delay(INIT_FAILURE_DELAY);
        }
        digitalWrite(REDLED, LED_OFF);
        Serial.println("SD Init Done.");
    }

    void init_tcs() {
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

    }

    ////////////////////////////////////////////////////////////
    //////////////////// RTC FUNCTIONS ////////////////////
    ////////////////////////////////////////////////////////////

    void init_rtc() {
        // RTC
        Serial.print("Initializing RTC.");
        if (!globals::rtc.begin()) {
            digitalWrite(REDLED, LED_ON);
            Serial.println("Couldn't find RTC");
            delay(INIT_FAILURE_DELAY);
        }
        globals::rtcMillis.begin(globals::rtc.now());
        digitalWrite(REDLED, LED_OFF);
        Serial.println("RTC Init Done.");
    }

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

                globals::rtc.adjust(DateTime(year, month, day, hour, minute, second));
                globals::rtcMillis.adjust(globals::rtc.now());

                // Read time
                Serial.println("Real Time Clock Updated");
                print_serial_date();

            } else {
                S.println("Invalid date-time format. Expected YYYYMMDDHHMMSS.");
            }
        }
    }

    String get_datetime() {
        //See RTC_millis synching here: https://forums.adafruit.com/viewtopic.php?p=154467#p154467

        static DateTime now = globals::rtcMillis.now();
        static long nowMillis = millis();
        static char buffer [31] = "";
        sprintf(
                buffer, 
                "%04d-%02d-%02dT%02d:%02d:%02d:%03dZ", 
                now.year(), 
                now.month(), 
                now.day(), 
                now.hour(), 
                now.minute(), 
                now.second(), 
                (nowMillis - globals::rtcMillisOffset) % 1000
            );
        return String(buffer);
    }        
}