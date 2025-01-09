#include "data.h"
#include <sstream>

namespace data {

    const char* settings_default = R"(
SET initial_wait_s 60
SET measures 60
SET period_ms 60000
META depth 0.5
META name Kdupro01
META buoy 1
META country Colombia
META place Akuara
META maker UDEA
META curator ICM-CSIC
META email rodero@icm.csic.es
META sensors TCS34725
META description calibration
META units counts, counts, counts, counts
META latitude 41.383189
META longitude 2.197949
META altitude 0
META ref_coord_system WGS84
META location_source GNSS
META time_source internet time pool
META processing_level 0
META processing_procedure https://git.csic.es/kduino/kdupro
META processing_version build
META processing_revision 0
META calibration_procedure https://git.csic.es/kduino/kdupro
META calibration_reference 0
META calibration_time 0
META calibration_version 0
META sensor_id 
META platform_id 
META deployment_id 
META sample_id 
META observer_id 
META owner_contact jpiera@icm.csic.es
META operator_contact rodero@icm.csic.es
META license MIT License
META license_reference https://opensource.org/licenses/MIT
META embargo_date 
META datetime YYYY-MM-DDThh:mm:ssZ
)";

    void system_data_setup(){
        /* Load metadata and settings */
        load_settings();
    }

    void load_settings(bool isInitAllowed){
        /* Load settings from SD card */
        File settings_file = SD.open(globals::sd_settings_file);
        if (settings_file) {
            Serial.println("Reading in settings file");
            globals::sC.readSerial(settings_file);
            settings_file.close();
        } else if (isInitAllowed == true) {
            Serial.println("No settings file found, creating default settings");
            settings_file = SD.open(globals::sd_settings_file, FILE_WRITE);
            settings_file.print(settings_default);
            settings_file.close();
            delay(100);
            load_settings(false);
        } else if (isInitAllowed == false) {
            Serial.println("Error opening settings file");
        }
    }

    void save_settings(){
        if (SD.exists(globals::sd_settings_file)) {
            SD.remove(globals::sd_settings_file);
        }
                
        File sfile = sd_open_file_by_name(globals::sd_settings_file);

        sfile.print("SET initial_wait_s ");
        sfile.println(globals::initial_wait_s, DEC);
        sfile.print("SET measures ");
        sfile.println(globals::measures, DEC);
        sfile.print("SET period_ms ");
        sfile.println(globals::period_ms, DEC);
        
        //Iterate over metadata keys
        int metadataSize = globals::metadata.size();
        for (int i = 0; i < metadataSize; i++) {
            sfile.print("META ");
            sfile.print(String(globals::metadata.getKey(i)));
            sfile.print(" ");
            sfile.println(String(globals::metadata.getValue(i).data()));
        }

        sfile.close();
    }

    ////////////////////////////////////////////////////////////
    ////// GENERATE METADATA ID AND FILE NAME //////////////////
    ////////////////////////////////////////////////////////////

    void generate_metadata_id(){
        /* update the ids from metadata */

        String date = actions::get_datetime().substring(0, 10);
        String time_sample = actions::get_datetime().substring(11, 16);
        String rnd_number = String(random(0xffff), HEX);

        globals::sensor_id = WiFi.macAddress();
        globals::sensor_id.replace(":", "");
        globals::platform_id = *globals::metadata["buoy"].data() + String("_") + *globals::metadata["country"].data() + String("_") + *globals::metadata["place"].data();
        globals::deployment_id = globals::sensor_id + String("_") + globals::metadata["name"].data() + String("_") + globals::metadata["depth"].data() + String("_") + rnd_number;
        globals::sample_id = globals::platform_id + String("_") + time_sample;
        globals::observer_id = globals::metadata["operator_contact"].data() + String("_") + date + String("_") + time_sample;
        Serial.println("Metadata IDs updated");
    }

    void generate_filename(){
        /* generate name of the file */
        String date = actions::get_datetime().substring(0, 10);
        globals::filename = date + String("_buoy") + globals::platform_id + String("_") + globals::metadata["depth"].data() + String(".txt");
        Serial.print("Filename updated: ");
        Serial.println(globals::filename);
    }

    File sd_open_file_by_name(String filename) {
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        File data_file = SD.open(filename, FILE_WRITE);

        while (! data_file) {
            digitalWrite(REDLED, LED_ON);
            // Wait forever since we cant write data
            Serial.println("error opening " + filename);
            delay(10000);
            data_file = SD.open(filename, FILE_WRITE);
        }
        digitalWrite(REDLED, LED_OFF);   

        return data_file;    
    }

}