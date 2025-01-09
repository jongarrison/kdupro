
#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <SD.h>
#include <Arduino.h>
#include "sCommand.h"
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include "globals.h"
#include "data.h"

namespace actions {
    void init_rtc();
    void init_sd();
    void init_tcs();

    void registerSerialCommands();
    
    void measure_TCS34725();
    void print_serial_rgb_data();
    void print_serial_metadata();
    void print_serial_header();
    void print_serial_date();
    void sd_save_data();
    void sd_save_new_line();
    void sd_save_date();
    void sd_save_metadata();
    void print_metadata_to_stream(Stream& S);
    void save_header();
    void start_data_recording_process();

    String get_datetime();

    struct Commands {
        const char *SET_RTC = "T";  //Set the RTC
        const char *GET_RTC = "GT"; //Get the current RTC value
        const char *LS_SD = "LS";   //List all files on SD
        const char *CAT_SD = "CAT"; //List contents of a file on SD
        const char *SIZE_SD = "SZ"; //Get the size of a file on SD
        const char *RM_SD = "RM";   //Remove a file on SD
        const char *SET = "SET";    //Set a specific run configuration value   
        const char *META = "META";  //Set some generic metadata string value
        const char *SAVE = "SAVE";  //Save the metadata into a file
        const char *PAUSE = "PAUSE"; //Pause the data recording
        const char *RESUME = "RESUME"; //Resume the data recording
        const char *START = "START"; //Resume the data recording
    };
    extern Commands commands; 

    void cmd_update_rtc_serial(sCommand& sC, Stream& S);
    void cmd_get_rtc_serial(sCommand& sC, Stream& S);
    void cmd_list_sd_files(sCommand& sC, Stream& S);
    void cmd_list_sd_file_contents(sCommand& sC, Stream& S);
    void cmd_list_sd_file_size(sCommand& sC, Stream& S);
    void cmd_rm_sd_file(sCommand& sC, Stream& S);
    void cmd_set_variable(sCommand& sC, Stream& S);
    void cmd_set_metadata(sCommand& sC, Stream& S);
    void cmd_sd_save_config_to_file(sCommand& sC, Stream& S);
    void cmd_pause_data(sCommand& sC, Stream& S);
    void cmd_resume_data(sCommand& sC, Stream& S);
    void cmd_start_data(sCommand& sC, Stream& S);
}

#endif /* ACTIONS_H_ */