# Values derived from other metadata values:

String sensor_id = "";              // Unique identifiers used to prevent data duplication with data consumers
String platform_id = "";            // Platform serial number or randomly assigned identifier (UUID) used with all connected sensors. May be left empty if not applicable.
String deployment_id = "";          // Randomly assigned identifier (UUID) specific to deployment sequence (e.g. cruise, campaign, vertical profile) of a specific sensor. Not shared with other sensors.
String sample_id = "";              // Randomly assigned identifier (UUID) generated with each distinct data record from any set of sensors belonging to a single observation.
String observer_id = "";            // Randomly assigned identifier (UUID) repeated with each data record from this and/or other sensors when operated by a specific observer.


# Original settings and metadata values:

// Settings 
int initial_wait_s = 60;               // Time to wait before start the loop (in seconds)

int measures = 60;                   // Number of measurements to do[1, 59]
int period_ms = 60000;                      // Sampling period (ms)
int sample_counter = 1;              // Counter of measurements

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