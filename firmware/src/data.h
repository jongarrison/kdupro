#ifndef DATA_H_
#define DATA_H_

#include "globals.h"
#include <SD.h>
#include "actions.h"

namespace data {
    void system_data_setup();
    void load_settings(bool isInitAllowed=true);
    void save_settings();

    void generate_metadata_id();
    void generate_filename();

    File sd_open_file_by_name(String filename);
}

#endif /* DATA_H_ */