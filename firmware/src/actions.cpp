#include "actions.h"

namespace actions {

    Commands commands;

    void registerSerialCommands() {
        globals::sC.addCommand(commands.LS_SD, cmd_list_sd_files);
        globals::sC.addCommand(commands.CAT_SD, cmd_list_sd_file_contents);
        globals::sC.addCommand(commands.SET_RTC, cmd_update_rtc_serial);
        globals::sC.addCommand(commands.GET_RTC, cmd_get_rtc_serial);
        globals::sC.addCommand(commands.SIZE_SD, cmd_list_sd_file_size);
        globals::sC.addCommand(commands.RM_SD, cmd_rm_sd_file);
        globals::sC.addCommand(commands.SET, cmd_set_variable);
        globals::sC.addCommand(commands.META, cmd_set_metadata);
        globals::sC.addCommand(commands.SAVE, cmd_sd_save_config_to_file);
        globals::sC.addCommand(commands.PAUSE, cmd_pause_data);
        globals::sC.addCommand(commands.RESUME, cmd_resume_data);
        globals::sC.addCommand(commands.START, cmd_start_data);
        Serial.println("Serial commands registered");
    }

    void sendCmdResponseFinish(Stream& S) {
        S.println("EOF");
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
            S.println(String(globals::metadata.getValue(i).data()));
        }

        S.print("time: ");
        S.println(get_datetime());
        S.flush();
    }

    void cmd_sd_save_config_to_file(sCommand& sC, Stream& S){
        data::save_settings();
        S.println("SAVED");
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
        return data::sd_open_file_by_name(globals::filename);
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

    void cmd_list_sd_files(sCommand& sC, Stream& S){
        File root = SD.open("/");
        while (true) {
            File entry = root.openNextFile();
            if (!entry) {
                // no more files
                break;
            }
            S.println(entry.name());
            entry.close();
        }
        sendCmdResponseFinish(S);
    }

    void cmd_list_sd_file_contents(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            Serial.println("Missing file name");
        } else {
            const char* filename = sC.current();
            File data_file = SD.open(filename);
            if (data_file) {
                while (data_file.available()) {
                    S.write(data_file.read());
                }
                sendCmdResponseFinish(S);
                data_file.close();
            } else {
                Serial.println("Error opening file");
            }
        }
    }

    void cmd_list_sd_file_size(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            Serial.println("Missing file name");
        } else {
            const char* filename = sC.current();
            File data_file = SD.open(filename);
            if (data_file) {
                S.println(data_file.size());
                data_file.close();
            } else {
                Serial.println("Error opening file");
            }
        }
        sendCmdResponseFinish(S);
    }

    void cmd_rm_sd_file(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            Serial.println("Missing file name");
        } else {
            const char* filename = sC.current();
            if (SD.remove(filename)) {
                Serial.println("File removed");
            } else {
                Serial.println("Error removing file");
            }
        }
    }

    void cmd_set_variable(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            Serial.println("Missing variable name");
        } else {
            const char* variableName = sC.current();
            if ( sC.next() == NULL) {
                Serial.println("Missing variable value");
            } else {
                // SET initial_wait_s 60
                // SET measures 60
                // SET period_ms 60000                
                const char* value = sC.current();
                Serial.print("SET ");
                Serial.print(variableName);
                Serial.print(":");

                if (strcmp(variableName, "initial_wait_s") == 0) {
                    globals::initial_wait_s = atoi(value);
                    Serial.println(globals::initial_wait_s);
                } else if (strcmp(variableName, "measures") == 0) {
                    globals::measures = atoi(value);
                    Serial.println(globals::measures);
                } else if (strcmp(variableName, "period_ms") == 0) {
                    globals::period_ms = atoi(value);  
                    Serial.println(globals::period_ms);                  
                } else {
                    Serial.print("Unknown variable name received:");
                    Serial.println(variableName);
                }
            }
        }
    }

    void cmd_set_metadata(sCommand& sC, Stream& S){
        if ( sC.next() == NULL) {
            Serial.println("Missing metadata name");
        } else {
            const char* variableName = sC.current();
            Serial.print("META ");
            Serial.print(variableName);
            Serial.print(":");

            if ( sC.next() == NULL) {
                strcpy(globals::metadata[variableName].data(), "");
            } else {
                strcpy(globals::metadata[variableName].data(), sC.current());
            }
            Serial.println(globals::metadata[variableName].data());
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
        digitalWrite(REDLED, LED_OFF);
        Serial.println("RTC Init Done.");
    }

    void cmd_get_rtc_serial(sCommand& sC, Stream& S) {
        print_serial_date();
        sendCmdResponseFinish(S);
    }

    void cmd_update_rtc_serial(sCommand& sC, Stream& S) {

        if ( sC.next() == NULL) {
            Serial.println("Missing new RTC time value (YYYYMMDDHHMMSS)");
        } else {
            const char* dateTimeStr = sC.current();
            Serial.printf("Received: %s\n", dateTimeStr);
            int year, month, day, hour, minute, second;

            // Parse the date-time string
            if (sscanf(dateTimeStr, "%4d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &minute, &second) == 6) {
                // Successfully parsed all components
                Serial.printf("Parsed date-time: %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
            
                // Now you can use the parsed values to update the RTC
                // Example: rtc.adjust(DateTime(year, month, day, hour, minute, second));

                globals::rtc.adjust(DateTime(year, month, day, hour, minute, second));

                // Read time
                Serial.println("Real Time Clock Updated");
                print_serial_date();

            } else {
                Serial.println("Invalid date-time format. Expected YYYYMMDDHHMMSS.");
            }
        }
    }

    String get_datetime() {
        //See RTC_millis synching here: https://forums.adafruit.com/viewtopic.php?p=154467#p154467
        /*
        * The RTClib library does not provide a way to get the milliseconds from the RTC.
        * So this is a workaround to get the milliseconds from the arduino, but the rest of the time from the RTC.
        */
        DateTime now = globals::rtc.now();
        long nowMillis = millis();
        char buffer [31] = "";
        sprintf(
                buffer, 
                "%04d-%02d-%02dT%02d:%02d:%02d:%03dZ", 
                now.year(), 
                now.month(), 
                now.day(), 
                now.hour(), 
                now.minute(), 
                now.second(), 
                nowMillis % 1000
            );
        return String(buffer);
    }    

    void print_serial_date(){
        /*It sends the data throught the serial  communication*/
        Serial.println(get_datetime());
    }    

    ////////////////////////////////////////////////////////////
    ///////////// DATA RECORDING MANAGEMENT ////////////////////
    ////////////////////////////////////////////////////////////

    void do_sample_collection() {
        if (globals::isDataCollectionPaused) {
            return;
        }

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
    }

    void start_data_recording_process() {
        if (globals::isDataCollectionPaused) {
            //Serial.println("Data collection is paused. Please resume before starting.");
            return;
        }

        data::generate_metadata_id();
        data::generate_filename();

        if (SD.exists(globals::filename)) {
            Serial.print("Data file already exists. Appending to it. WARNING if metadata has changed, it won't be captured in file");
        } else {
            // Write metadata and header into file.txt
            actions::sd_save_metadata();
            actions::save_header();
            // Send metadata and header through serial communication
            actions::print_serial_metadata();
            actions::print_serial_header();
        }

        //Set up the actual sample collection schedule
        if (globals::mainTimerId != 0) {
            globals::timer.deleteTimer(globals::mainTimerId);
        }
        globals::mainTimerId = globals::timer.setInterval(globals::period_ms, do_sample_collection);        
    }

    void cmd_pause_data(sCommand& sC, Stream& S) {
        globals::isDataCollectionPaused = true;
        S.println("PAUSED");
        sendCmdResponseFinish(S);
    }

    void cmd_resume_data(sCommand& sC, Stream& S) {
        globals::isDataCollectionPaused = false;
        if (globals::mainTimerId == 0) {
            cmd_start_data(sC, S);
        } else {
            S.println("RESUMED");
            sendCmdResponseFinish(S);
        }
    }

    void cmd_start_data(sCommand& sC, Stream& S) {
        start_data_recording_process();
        S.println("STARTED");
        sendCmdResponseFinish(S);
    }



}