
#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <SD.h>
#include <Arduino.h>
#include "sCommand.h"
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include "globals.h"

namespace actions {
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
    void print_metadata_to_stream(Stream& S);
    void save_header();

    void generate_metadata_id();
    void generate_filename();
    String get_datetime();

    void list_sd_files(sCommand& sC, Stream& S);
    void list_sd_file_contents(sCommand& sC, Stream& S);
    void update_rtc_serial(sCommand& sC, Stream& S);
    void get_rtc_serial(sCommand& sC, Stream& S);    
}

#endif /* ACTIONS_H_ */