#include "actions.h"

namespace actions {

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
        deployment_id = sensor_id + "_" + name + "_" + globals::depth + "_" + rnd_number;
        sample_id = platform_id + "_" + time_sample;
        observer_id = operator_contact + "_" + date + "_" + time_sample;
    }

    void generate_filename(){
        /* generate name of the file */
        String date = get_datetime().substring(0, 10);
        globals::filename = date + "_" + "buoy" +  platform_id + "_" + globals::depth + ".txt";
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
        S.print("depth: ");
        S.println(globals::depth);
        S.print("sample_counter: ");
        S.println(globals::sample_counter);
        
        S.print("name: ");
        S.println(name);
        S.print("buoy: ");
        S.println(buoy);
        S.print("place: ");
        S.println(place);
        S.print("country: ");
        S.println(country);
        S.print("maker: ");
        S.println(maker);
        S.print("curator: ");
        S.println(curator);
        S.print("email: ");
        S.println(email);
        S.print("sensors: ");
        S.println(sensors);
        S.print("description: ");
        S.println(description);
        S.print("units: ");
        S.println(units);

        S.print("latitude: ");
        S.println(latitude);
        S.print("longitude: ");
        S.println(longitude);
        S.print("altitude: ");
        S.println(altitude);
        S.print("ref_coord_system: ");
        S.println(ref_coord_system);
        S.print("location_source: ");
        S.println(location_source);
        S.print("time_source: ");
        S.println(time_source);
        S.print("processing_level: ");
        S.println(processing_level);
        S.print("processing_procedure: ");
        S.println(processing_procedure);
        S.print("processing_version: ");
        S.println(processing_version);
        S.print("processing_revision: ");
        S.println(processing_revision);
        S.print("calibration_procedure: ");
        S.println(calibration_procedure);
        S.print("calibration_reference: ");
        S.println(calibration_reference);
        S.print("calibration_time: ");
        S.println(calibration_time);
        S.print("calibration_version: ");
        S.println(calibration_version);
        S.print("sensor_id: ");
        S.println(sensor_id);
        S.print("platform_id: ");
        S.println(platform_id);
        S.print("deployment_id: ");
        S.println(deployment_id);
        S.print("sample_id: ");
        S.println(sample_id);
        S.print("observer_id: ");
        S.println(observer_id);
        S.print("owner_contact: ");
        S.println(owner_contact);
        S.print("operator_contact: ");
        S.println(operator_contact);
        S.print("license: ");
        S.println(license);
        S.print("license_reference: ");
        S.println(license_reference);
        S.print("embargo_date: ");
        S.println(embargo_date);

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
        print_metadata_to_stream(data_file);
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

        globals::now = globals::rtc.now();
        char buffer [31] = "";
        sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d:%03dZ", globals::now.year(), globals::now.month(), globals::now.day(), globals::now.hour(), globals::now.minute(), globals::now.second(), millis() % 1000);
        datetime = String(buffer);

        return datetime;
    }

}