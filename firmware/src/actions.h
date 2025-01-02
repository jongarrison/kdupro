
#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <SD.h>
#include <Arduino.h>
#include "sCommand.h"
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include "globals.h"

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

    String get_datetime();

    struct Commands {
        const char *SET_RTC = "T";
        const char *GET_RTC = "GT";
        const char *LS_SD = "LS";
        const char *CAT_SD = "CAT";
        const char *SIZE_SD = "SZ";
        const char *RM_SD = "RM";
        const char *SET = "SET";
        const char *META = "META";
    };
    extern Commands commands; 

    void update_rtc_serial(sCommand& sC, Stream& S);
    void get_rtc_serial(sCommand& sC, Stream& S);
    void list_sd_files(sCommand& sC, Stream& S);
    void list_sd_file_contents(sCommand& sC, Stream& S);
    void list_sd_file_size(sCommand& sC, Stream& S);
    void rm_sd_file(sCommand& sC, Stream& S);
    void set_variable(sCommand& sC, Stream& S);
    void set_metadata(sCommand& sC, Stream& S);

}

#endif /* ACTIONS_H_ */